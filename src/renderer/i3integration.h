#pragma once
#include "math/mat.h"
#include "math/vec.h"
#include "memory/arena.h"
#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct Im3dVkInitInfo
{
    VkDevice device;
    VkPhysicalDevice physical_device;
    VkExtent2D swapchain_extent;
    VkRenderPass render_pass;
    VkCommandPool command_pool;
    VkQueue graphics_queue;

    m4 projection_matrix;
} Im3dVkInitInfo;

bool im3dVkInit(Arena* arena, Im3dVkInitInfo info);
void im3dVkShutdown(void);
void im3dVkNewFrame(void);
void im3dVkEndFrame(Arena* arena, VkCommandBuffer cmd_buffer);
void im3dVkSetCamera(v3 position, v3 direction);