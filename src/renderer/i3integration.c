#include "i3integration.h"
#include "buffer.h"
#include "error.h"
#include "input.h"
#include "log/log.h"
#include "math/mat.h"
#include "math/scalar.h"
#include "memory/stackallocator.h"
#include "types.h"
#include <cim3d.h>
#include <math.h>
#include <cimgui.h>

#define VERTEX_BUFFER_SIZE (0x8000 * sizeof(im3dVertexData))

typedef struct PushConstantsObject
{
    m4 proj_view;
} PushConstantsObject;

static struct Im3dVkContext
{
    VkDevice         device;
    VkPhysicalDevice physical_device;
    VkExtent2D       swapchain_extent;
    VkRenderPass     render_pass;
    VkCommandPool    command_pool;
    VkQueue          graphics_queue;

    VkPipelineLayout pipeline_layout;
    VkBuffer         vertex_buffer;
    VkDeviceMemory   vertex_buffer_memory;
    VkPipeline       points_pipeline;
    VkPipeline       lines_pipeline;
    VkPipeline       triangles_pipeline;

    PushConstantsObject push_constants;

    m4  projection_matrix;
    m4  view_matrix;
    v3  camera_position;
    v3  camera_direction;
} C;

static VkShaderModule CreateShaderModule(Buffer code)
{
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.length,
        .pCode = (uint32_t*)code.ptr
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(C.device, &createInfo, NULL, &shaderModule)
        != VK_SUCCESS)
            ERROR("Shader module creation failure.");

    return shaderModule;
}

static VkVertexInputBindingDescription GetBindingDescription(void)
{
    VkVertexInputBindingDescription binding_description = {
        .binding = 0,
        .stride = sizeof(im3dVertexData),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    return binding_description;
}

static void GetAttributeDescriptions(VkVertexInputAttributeDescription* descs, u8* num)
{
    static VkVertexInputAttributeDescription attribute_descriptions[] = {
        [0] = {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(im3dVertexData, m_positionSize)
        },
        [1] = {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_A8B8G8R8_UNORM_PACK32,
            .offset = offsetof(im3dVertexData, m_color)
        }
    };

    memcpy(descs, attribute_descriptions, sizeof(attribute_descriptions));
    *num = ArrayCount(attribute_descriptions);
}

static VkPipeline CreateGraphicsPipeline(const char** shader_files, u32 shader_num,
                                         VkPrimitiveTopology topology)
{
    enum
    {
        VERTEX_SHADER = 0,
        FRAGMET_SHADER = 1,
        GEOMETRY_SHADER = 2,

        MAX_SHADER_STAGES = 3
    };

    VkShaderModule shader_modules[MAX_SHADER_STAGES];
    for (u32 i = 0; i < shader_num; i++)
    {
        Buffer shader_code = File2Buffer(shader_files[i]);
        shader_modules[i] = CreateShaderModule(shader_code);
        FreeBuffer(shader_code);
    }

    VkPipelineShaderStageCreateInfo shader_stages[MAX_SHADER_STAGES] = {
        [VERTEX_SHADER] = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shader_modules[VERTEX_SHADER],
            .pName = "main",
        },
        [FRAGMET_SHADER] = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shader_modules[FRAGMET_SHADER],
            .pName = "main",
        }
    };

    if (shader_num == 3)
    {
        VkPipelineShaderStageCreateInfo geometry = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_GEOMETRY_BIT,
            .module = shader_modules[GEOMETRY_SHADER],
            .pName = "main",
        };
        shader_stages[GEOMETRY_SHADER] = geometry;
    }

    VkVertexInputBindingDescription binding_desc = GetBindingDescription();
    u8 attribute_num = 0;
    VkVertexInputAttributeDescription attribute_descs[2];
    GetAttributeDescriptions(attribute_descs, &attribute_num);

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding_desc,
        .vertexAttributeDescriptionCount = ArrayCount(attribute_descs),
        .pVertexAttributeDescriptions = attribute_descs,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) C.swapchain_extent.width,
        .height = (float) C.swapchain_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
        .offset  = {0, 0},
        .extent = C.swapchain_extent,
    };

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
    };

    // Requires enabling GPU Feature.
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachement = {
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, // or VK_BLEND_FACTOR_ONE
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachement,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f,
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
        .stencilTestEnable = VK_FALSE,
    };

    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shader_num,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depth_stencil,
        .pColorBlendState = &color_blending,
        .pDynamicState = NULL,
        .layout = C.pipeline_layout,
        .renderPass = C.render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(C.device, VK_NULL_HANDLE, 1, &pipeline_info,
        NULL, &pipeline) != VK_SUCCESS)
        ERROR("Graphics Pipelines creation failure");

    L_INFO("Graphics Pipeline has been created.");

    for (u32 i = 0; i < shader_num; i++)
    {
        vkDestroyShaderModule(C.device, shader_modules[i], NULL);
    }

    return pipeline;
}

static void CreatePipelineLayout()
{
    VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(PushConstantsObject)
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &push_constant_range,
    };

    if (vkCreatePipelineLayout(C.device, &pipeline_layout_info, NULL,
        &C.pipeline_layout) != VK_SUCCESS)
        ERROR("PipelineLayout creation failure.");
}

static CreatePipelines()
{
    const char* line_shaders[] = {
        "shaders/im3d.lines.vert.spv",
        "shaders/im3d.lines.frag.spv",
        "shaders/im3d.lines.geom.spv"
    };
    C.lines_pipeline = CreateGraphicsPipeline(
        line_shaders, ArrayCount(line_shaders), VK_PRIMITIVE_TOPOLOGY_LINE_LIST);

    const char* point_shaders[] = {
        "shaders/im3d.points.vert.spv",
        "shaders/im3d.points.frag.spv"
    };
    C.points_pipeline = CreateGraphicsPipeline(
        point_shaders, ArrayCount(point_shaders), VK_PRIMITIVE_TOPOLOGY_POINT_LIST);

    const char* triangle_shaders[] = {
        "shaders/im3d.triangles.vert.spv",
        "shaders/im3d.triangles.frag.spv"
    };
    C.triangles_pipeline = CreateGraphicsPipeline(
        triangle_shaders, ArrayCount(triangle_shaders), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
}

static u32 FindMemoryType(u32 type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(C.physical_device, &mem_properties);

    for (u32 i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if (type_filter & (1 << i) &&
           (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    ERROR("Failed to find a suitable memory type");
    return 0;
}

static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory)
{
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(C.device, &buffer_info, NULL, buffer)
        != VK_SUCCESS)
    {
        ERROR("Vertex buffer creation failure");
    }

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(C.device, *buffer, &memory_requirements);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits,
            properties)
    };

    if (vkAllocateMemory(C.device, &alloc_info, NULL, buffer_memory)
        != VK_SUCCESS)
    {
        ERROR("Vertex buffer memory allocation failed");
    }

    // Associate buffer with a memory region
    vkBindBufferMemory(C.device, *buffer, *buffer_memory, 0);
}

static void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = C.command_pool,
        .commandBufferCount = 1
    };

    VkCommandBuffer cmd_buffer;
    vkAllocateCommandBuffers(C.device, &alloc_info, &cmd_buffer);

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkBufferCopy copy_region = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size
    };
    vkCmdCopyBuffer(cmd_buffer, src_buffer, dst_buffer, 1, &copy_region);

    vkEndCommandBuffer(cmd_buffer);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buffer
    };

    vkQueueSubmit(C.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(C.graphics_queue);

    vkFreeCommandBuffers(C.device, C.command_pool, 1, &cmd_buffer);
}

static void CreateVertexBuffer(void)
{
    CreateBuffer(VERTEX_BUFFER_SIZE, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &C.vertex_buffer, &C.vertex_buffer_memory);
}

static void UploadToVertexBuffer(void* input, VkDeviceSize offset, VkDeviceSize size)
{
    assert(size < VERTEX_BUFFER_SIZE);
    void* data;
    vkMapMemory(C.device, C.vertex_buffer_memory, offset, size, 0, &data);
    memcpy(data, input, size);
    vkUnmapMemory(C.device, C.vertex_buffer_memory);
}

static InitContext(Im3dVkInitInfo info)
{
    C.device = info.device;
    C.render_pass = info.render_pass;
    C.swapchain_extent = info.swapchain_extent;
    C.command_pool = info.command_pool;
    C.graphics_queue = info.graphics_queue;
    C.physical_device = info.physical_device;
    C.projection_matrix = info.projection_matrix;
}

bool im3dVkInit(Im3dVkInitInfo info)
{
    InitContext(info);
    CreateVertexBuffer();
    CreatePipelineLayout();
    CreatePipelines();
    return true;
}

void im3dVkShutdown(void)
{
    vkDestroyBuffer(C.device, C.vertex_buffer, NULL);
    vkFreeMemory(C.device, C.vertex_buffer_memory, NULL);
    vkDestroyPipelineLayout(C.device, C.pipeline_layout, NULL);
    vkDestroyPipeline(C.device, C.points_pipeline, NULL);
    vkDestroyPipeline(C.device, C.lines_pipeline, NULL);
    vkDestroyPipeline(C.device, C.triangles_pipeline, NULL);
}

void im3dVkNewFrame(void)
{
    im3dAppData* ad = im3dGetAppData();
    ad->m_deltaTime     = 0.0f; // unused
    ad->m_viewportSize  = v2(C.swapchain_extent.width, C.swapchain_extent.height);
    ad->m_viewOrigin    = C.camera_position;
    ad->m_viewDirection = C.camera_direction;
    ad->m_worldUp       = v3(0.0f, 1.0f, 0.0f);
    ad->m_projOrtho     = false;
    ad->m_projScaleY    = tanf(radians(90.0f) * 0.5f) * 2.0f;

    v2 cursor_pos = GetMousePosition();
    cursor_pos.x = (cursor_pos.x / ad->m_viewportSize.width) * 2.0f - 1.0f;
    cursor_pos.y = (cursor_pos.y / ad->m_viewportSize.height) * 2.0f - 1.0f;
    v3 ray_origin, ray_direction;

    ray_origin          = ad->m_viewOrigin;
    ray_direction.x     = cursor_pos.x / C.projection_matrix.elems[0][0];
    ray_direction.y     = cursor_pos.y / C.projection_matrix.elems[1][1];
    ray_direction.z     = -1.0f;
    ray_direction       = V3Normalize(ray_direction);
    v4 ray_direction_v4 = M4MultiplyV4(C.view_matrix, v4(ray_direction));
    ray_direction       = v3(ray_direction_v4);

    ad->m_cursorRayOrigin = ray_origin;
    ad->m_cursorRayDirection = ray_direction;

    ad->m_keyDown[im3dKey_Mouse_Left] = GetMouseButtonState(BUTTON_LEFT) != KEY_STATE_UP;
    bool ctrl_down = GetKeyState(KEY_CTRL) != KEY_STATE_UP;
    ad->m_keyDown[im3dKey_Key_L] = ctrl_down && GetKeyState(KEY_L) & KEY_STATE_PRESSED;
    ad->m_keyDown[im3dKey_Key_T] = ctrl_down && GetKeyState(KEY_T) & KEY_STATE_PRESSED;
    ad->m_keyDown[im3dKey_Key_R] = ctrl_down && GetKeyState(KEY_R) & KEY_STATE_PRESSED;
    ad->m_keyDown[im3dKey_Key_S] = ctrl_down && GetKeyState(KEY_S) & KEY_STATE_PRESSED;

    ad->m_snapTranslation = ctrl_down ? 0.5f : 0.0f;
    ad->m_snapRotation    = ctrl_down ? radians(30.0f) : 0.0f;
    ad->m_snapScale       = ctrl_down ? 0.5f : 0.0f;

    // TODO(Sand3r-): Add setCullFrustum binding
    im3dNewFrame();
}

void im3dVkEndFrame(VkCommandBuffer cmd_buffer)
{
    im3dEndFrame();

    // Total number of vertices per each primitive type
    u32 vertex_counts[DrawPrimitive_Count] = {0};
    // Total number of primitives per each type
    u32 prim_counts[DrawPrimitive_Count] = {0};

    // First scan through all the draw calls to find out vertex and primitive counts
    for (u32 i = 0; i < im3dGetDrawListCount(); i++)
    {
        const im3dDrawList* draw_list = &im3dGetDrawLists()[i];
        vertex_counts[draw_list->m_primType] += draw_list->m_vertexCount;
        prim_counts[draw_list->m_primType]++;
    }

    // Index of the first vertex per each primitive type
    u32 index[DrawPrimitive_Count] = {
        [DrawPrimitive_Triangles] = 0,
        [DrawPrimitive_Lines] = vertex_counts[DrawPrimitive_Triangles],
        [DrawPrimitive_Points] = (vertex_counts[DrawPrimitive_Triangles]
                               + vertex_counts[DrawPrimitive_Lines])
    };

    // Byte offets to the Vertex Buffer per each primitive type
    u32 offsets[DrawPrimitive_Count] = {
        [DrawPrimitive_Triangles] = index[DrawPrimitive_Triangles] * sizeof(im3dVertexData),
        [DrawPrimitive_Lines] = index[DrawPrimitive_Lines] * sizeof(im3dVertexData),
        [DrawPrimitive_Points] = index[DrawPrimitive_Points] * sizeof(im3dVertexData)
    };

    typedef struct DrawCallInfo
    {
        u32 vertex_index;
        u32 vertex_count;
    } DrawCallInfo;

    DrawCallInfo* draw_infos[DrawPrimitive_Count] = {
        [DrawPrimitive_Triangles] = StackMalloc(
            prim_counts[DrawPrimitive_Triangles] * sizeof(DrawCallInfo),
            "im3d Triangle DrawCall list"),
        [DrawPrimitive_Lines] = StackMalloc(
            prim_counts[DrawPrimitive_Lines] * sizeof(DrawCallInfo),
            "im3d Line DrawCall list"),
        [DrawPrimitive_Points] = StackMalloc(
            prim_counts[DrawPrimitive_Points] * sizeof(DrawCallInfo),
            "im3d Point DrawCall list"),
    };

    u32 draw_info_sizes[DrawPrimitive_Count] = {0};

    // Copy all the vertices to the vertex buffer, construct draw call information
    for (u32 i = 0; i < im3dGetDrawListCount(); i++)
    {
        const im3dDrawList* draw = &im3dGetDrawLists()[i];
        VkDeviceSize size = draw->m_vertexCount * sizeof(im3dVertexData);
        UploadToVertexBuffer(draw->m_vertexData, offsets[draw->m_primType], size);
        offsets[draw->m_primType] += size;
        DrawCallInfo info = {
            .vertex_index = index[draw->m_primType],
            .vertex_count = draw->m_vertexCount
        };
        draw_infos[draw->m_primType][draw_info_sizes[draw->m_primType]++] = info;
        index[draw->m_primType] += draw->m_vertexCount;
    }

    VkDeviceSize buffer_offset = 0;
    vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &C.vertex_buffer, &buffer_offset);
    vkCmdPushConstants(cmd_buffer, C.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT,
        0, sizeof(PushConstantsObject), &C.push_constants);
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, C.triangles_pipeline);
    for (u32 i = 0; i < draw_info_sizes[DrawPrimitive_Triangles]; i++)
    {
        vkCmdDraw(cmd_buffer, draw_infos[DrawPrimitive_Triangles][i].vertex_count,
                  1, draw_infos[DrawPrimitive_Triangles][i].vertex_index, 0);
    }

    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, C.lines_pipeline);
    for (u32 i = 0; i < draw_info_sizes[DrawPrimitive_Lines]; i++)
    {
        vkCmdDraw(cmd_buffer, draw_infos[DrawPrimitive_Lines][i].vertex_count,
                  1, draw_infos[DrawPrimitive_Lines][i].vertex_index, 0);
    }

    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, C.points_pipeline);
    for (u32 i = 0; i < draw_info_sizes[DrawPrimitive_Points]; i++)
    {
        vkCmdDraw(cmd_buffer, draw_infos[DrawPrimitive_Points][i].vertex_count,
                  1, draw_infos[DrawPrimitive_Points][i].vertex_index, 0);
    }

    StackFree(draw_infos[DrawPrimitive_Points]);
    StackFree(draw_infos[DrawPrimitive_Lines]);
    StackFree(draw_infos[DrawPrimitive_Triangles]);
}

void im3dVkSetCamera(v3 position, v3 direction)
{
    v3 up = v3(0.0f, 1.0f, 0.0f);
    C.view_matrix = LookAt(position, V3Add(position, direction), up);
    C.push_constants.proj_view = M4Multiply(C.projection_matrix, C.view_matrix);
    C.camera_position = position;
    C.camera_direction = direction;
}
