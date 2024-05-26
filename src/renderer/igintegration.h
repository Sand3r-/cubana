#pragma once
#include "types.h"

typedef struct Window Window;
typedef struct ImGui_ImplVulkan_InitInfo ImGui_ImplVulkan_InitInfo;
typedef struct VkRenderPass_T* VkRenderPass;
typedef struct VkCommandPool_T* VkCommandPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkDevice_T* VkDevice;
typedef struct VkQueue_T* VkQueue;
typedef union SDL_Event SDL_Event;

void ImGuiInit(Window window, ImGui_ImplVulkan_InitInfo init_info, VkRenderPass render_pass);
void ImGuiUploadFonts(VkDevice device, VkQueue queue, VkCommandPool command_pool, VkCommandBuffer command_buffer);
void ImGuiProcessEvents(SDL_Event event);
void ImGuiNewFrame(f32 delta);
void ImGuiRender(VkCommandBuffer command_buffer);
void ImGuiShutdown(void);