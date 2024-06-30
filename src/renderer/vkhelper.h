#pragma once
#include <vulkan/vulkan.h>

VkShaderModule VkShaderModuleFromFile(const char* filename, VkDevice device);
VkPipelineInputAssemblyStateCreateInfo VkCreateDefaultInputAssemblyInfo(VkPrimitiveTopology topology);
VkPipelineViewportStateCreateInfo VkCreateDefaultViewportInfo(VkViewport* viewport, VkRect2D* scissor);
VkPipelineRasterizationStateCreateInfo VkCreateDefaultRasterizationInfo(VkCullModeFlags cull_mode);
VkPipelineMultisampleStateCreateInfo VkCreateDefaultMultisampleInfo();
