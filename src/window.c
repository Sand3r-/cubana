#include "window.h"
#include "error.h"
#include <SDL.h>
#include <SDL_vulkan.h>

WindowResult CreateWindow(i32 width, i32 height, const char* name)
{
    int flags = SDL_WINDOW_SHOWN;
#ifdef USE_VK_RENDERER
    flags |= SDL_WINDOW_VULKAN;
#endif
    SDL_Window* handle = SDL_CreateWindow(
        name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (handle == NULL)
    {
        WindowResult window = {
            .valid = 0,
            .error_message = SDL_GetError()
        };
        return window;
    }

    WindowResult window = {
        .window = {
            ._internal = handle
        },
        .valid = 1
    };

    return window;
}

iv2 GetWindowSize(Window window)
{
    iv2 result;
    SDL_GetWindowSize(window._internal, &result.width, &result.height);
    return result;
}

b32 GetVulkanExtensions(Window window, u32* count, const char** names)
{
    return SDL_Vulkan_GetInstanceExtensions(window._internal, count, names);
}

b32 CreateVulkanSurface(Window window, VkInstance instance, VkSurfaceKHR *surface)
{
    return SDL_Vulkan_CreateSurface(window._internal, instance, surface);
}

void GetVulkanDrawableSize(Window window, i32* width, i32* height)
{
    SDL_Vulkan_GetDrawableSize(window._internal, width, height);
}

const char* GetWindowTitle(Window window)
{
    return SDL_GetWindowTitle(window._internal);
}

void DestroyWindow(Window window)
{
    SDL_DestroyWindow(window._internal);
}