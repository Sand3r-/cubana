#include "igintegration.h"
#include "buffer.h"
#include "window.h"
#include "error.h"
#include "imfont_storage.h"
#include "memory/arena.h"
#include "vkhelper.h"
#include <vulkan/vulkan.h>
#include <cimgui.h>
#include <cimgui_impl.h>
#include <SDL_events.h>
#include <stddef.h>

static struct ImGuiIntegrationData
{
    VkPipeline pipeline;
    VkRenderPass render_pass;
    u32 subpass;
} C;

static void ImGuiCreateGraphicsPipeline(Arena* arena, VkDevice device)
{
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSetLayoutBinding binding[1] = {
        [0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        [0].descriptorCount = 1,
        [0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = binding,
    };
    if (vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info,
                                    NULL, &descriptor_set_layout)
        != VK_SUCCESS)
    {
        ERROR("Descriptor Set Layout creation failure!");
    }

    VkPipelineLayout pipeline_layout;
    // Constants: we are using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix
    VkPushConstantRange push_constants[1] = {
        [0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        [0].offset = sizeof(float) * 0,
        [0].size = sizeof(float) * 4,
    };
    VkDescriptorSetLayout set_layout[1] = { descriptor_set_layout };
    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = push_constants,
    };
    if (vkCreatePipelineLayout(device, &layout_info, NULL, &pipeline_layout)
        != VK_SUCCESS)
    {
        ERROR("Pipeline Layout creation failure!");
    }

    VkShaderModule vertShaderModule = VkShaderModuleFromFile("shaders/imgui_shader.vert.spv", device);
    VkShaderModule fragShaderModule= VkShaderModuleFromFile("shaders/imgui_shader.frag.spv", device);

    VkPipelineShaderStageCreateInfo stage[2] = {
        [0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        [0].stage = VK_SHADER_STAGE_VERTEX_BIT,
        [0].module = vertShaderModule,
        [0].pName = "main",
        [1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        [1].stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        [1].module = fragShaderModule,
        [1].pName = "main",
    };

    VkVertexInputBindingDescription binding_desc[1] = {
        [0].stride = sizeof(ImDrawVert),
        [0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription attribute_desc[3] = {
        [0].location = 0,
        [0].binding = binding_desc[0].binding,
        [0].format = VK_FORMAT_R32G32_SFLOAT,
        [0].offset = offsetof(ImDrawVert, pos),
        [1].location = 1,
        [1].binding = binding_desc[0].binding,
        [1].format = VK_FORMAT_R32G32_SFLOAT,
        [1].offset = offsetof(ImDrawVert, uv),
        [2].location = 2,
        [2].binding = binding_desc[0].binding,
        [2].format = VK_FORMAT_R8G8B8A8_UNORM,
        [2].offset = offsetof(ImDrawVert, col),
    };

    VkPipelineVertexInputStateCreateInfo vertex_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = binding_desc,
        .vertexAttributeDescriptionCount = 3,
        .pVertexAttributeDescriptions = attribute_desc,
    };

    VkPipelineInputAssemblyStateCreateInfo ia_info = VkCreateDefaultInputAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    VkPipelineViewportStateCreateInfo viewport_info = VkCreateDefaultViewportInfo(NULL, NULL);
    VkPipelineRasterizationStateCreateInfo raster_info = VkCreateDefaultRasterizationInfo(VK_CULL_MODE_NONE);
    VkPipelineMultisampleStateCreateInfo ms_info = VkCreateDefaultMultisampleInfo();

    VkPipelineColorBlendAttachmentState color_attachment[1] = {
        [0].blendEnable = VK_TRUE,
        [0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        [0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        [0].colorBlendOp = VK_BLEND_OP_ADD,
        [0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        [0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        [0].alphaBlendOp = VK_BLEND_OP_ADD,
        [0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineDepthStencilStateCreateInfo depth_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
    };

    VkPipelineColorBlendStateCreateInfo blend_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = color_attachment,
    };

    VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (uint32_t)ArrayCount(dynamic_states),
        .pDynamicStates = dynamic_states,
    };

    VkGraphicsPipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .flags = 0,
        .stageCount = 2,
        .pStages = stage,
        .pVertexInputState = &vertex_info,
        .pInputAssemblyState = &ia_info,
        .pViewportState = &viewport_info,
        .pRasterizationState = &raster_info,
        .pMultisampleState = &ms_info,
        .pDepthStencilState = &depth_info,
        .pColorBlendState = &blend_info,
        .pDynamicState = &dynamic_state,
        .layout = pipeline_layout,
        .renderPass = C.render_pass,
        .subpass = C.subpass,
    };
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info,
        NULL, &C.pipeline) != VK_SUCCESS)
        ERROR("Graphics Pipelines creation failure");

    L_INFO("ImGui Graphics Pipeline has been created.");

    vkDestroyShaderModule(device, fragShaderModule, NULL);
    vkDestroyShaderModule(device, vertShaderModule, NULL);
}

void ImGuiInit(Arena* arena, Window window, ImGui_ImplVulkan_InitInfo init_info, VkRenderPass render_pass)
{
    igCreateContext(NULL);
    ImGuiIO* ioptr = igGetIO();
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForVulkan(window._internal);
    ImGui_ImplVulkan_Init(&init_info, render_pass);
    C.render_pass = render_pass;
    C.subpass = init_info.Subpass;
    ImGuiCreateGraphicsPipeline(arena, init_info.Device);
    igStyleColorsDark(NULL);
}

void ImGuiUploadFonts(VkDevice device, VkQueue queue, VkCommandPool command_pool,
                      VkCommandBuffer command_buffer)
{
    InitialiseImFontStorage();

    if (vkResetCommandPool(device, command_pool, 0) != VK_SUCCESS)
        ERROR("Command Pool couldn't have been resetted");

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
        ERROR("Command buffer couldn't start recording");

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo end_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
    };

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        ERROR("Command buffer recording faiulure");

    if (vkQueueSubmit(queue, 1, &end_info, VK_NULL_HANDLE) != VK_SUCCESS)
        ERROR("Queue submission failure");

    if (vkDeviceWaitIdle(device) != VK_SUCCESS)
        ERROR("Device Wait Idle failure");

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiProcessEvents(SDL_Event e)
{
    ImGui_ImplSDL2_ProcessEvent(&e);
}

void ImGuiNewFrame(f32 delta)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
    ImGuiIO* ioptr = igGetIO();
    ioptr->DeltaTime = delta / 1000.0f;
}

void ImGuiRender(VkCommandBuffer command_buffer)
{
    igRender();
    ImDrawData* draw_data = igGetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer, C.pipeline);
}

void ImGuiShutdown(void)
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(NULL);
}
