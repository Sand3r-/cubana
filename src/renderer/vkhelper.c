#include "vkhelper.h"
#include "buffer.h"
#include "error.h"
#include "memory/thread_scratch.h"

VkShaderModule VkShaderModuleFromFile(const char* filename, VkDevice device)
{
    Arena* arena = GetScratchArena();
    VkShaderModule result = VK_NULL_HANDLE;
    with_arena(arena)
    {
        Buffer code = BufferFromFile(arena, filename);
        VkShaderModuleCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.length,
            .pCode = (uint32_t*)code.ptr
        };

        if (vkCreateShaderModule(device, &createInfo, NULL, &result)
            != VK_SUCCESS)
                ERROR("Shader module creation failure.");
    }
    return result;
}

VkPipelineInputAssemblyStateCreateInfo VkCreateDefaultInputAssemblyInfo(VkPrimitiveTopology topology)
{
    VkPipelineInputAssemblyStateCreateInfo ia_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
    };

    return ia_info;
}

VkPipelineViewportStateCreateInfo VkCreateDefaultViewportInfo(VkViewport* viewport, VkRect2D* scissor)
{
    VkPipelineViewportStateCreateInfo viewport_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
        .pViewports = viewport,
        .pScissors = scissor,
    };

    return viewport_info;
}

VkPipelineRasterizationStateCreateInfo VkCreateDefaultRasterizationInfo(VkCullModeFlags cull_mode)
{
    VkPipelineRasterizationStateCreateInfo raster_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = cull_mode,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.0f,
    };

    return raster_info;
}

VkPipelineMultisampleStateCreateInfo VkCreateDefaultMultisampleInfo()
{
    VkPipelineMultisampleStateCreateInfo ms_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    return ms_info;
}

