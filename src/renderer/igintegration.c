#include "igintegration.h"
#include "window.h"
#include "error.h"
#include <vulkan/vulkan.h>
#include <cimgui.h>
#include <cimgui_impl.h>
#include <SDL_events.h>

void ImGuiInit(Window window, ImGui_ImplVulkan_InitInfo init_info, VkRenderPass render_pass)
{
    igCreateContext(NULL);
    ImGuiIO* ioptr = igGetIO();
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForVulkan(window._internal);
    ImGui_ImplVulkan_Init(&init_info, render_pass);
    igStyleColorsDark(NULL);
}

void ImGuiUploadFonts(VkDevice device, VkQueue queue, VkCommandPool command_pool,
                      VkCommandBuffer command_buffer)
{
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

void ImGuiNewFrame(void)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
}

void ImGuiRender(VkCommandBuffer command_buffer)
{
    igRender();
    ImDrawData* draw_data = igGetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer, VK_NULL_HANDLE);
}

void ImGuiShutdown(void)
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(NULL);
}
