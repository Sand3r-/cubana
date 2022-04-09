#pragma once
#include "types.h"
#include "math/vec.h"
#ifdef VULKAN_H_
#define NO_SDL_VULKAN_TYPEDEFS
#endif
#ifndef NO_SDL_VULKAN_TYPEDEFS
#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#else
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#endif

VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)

#endif


struct SDL_Window;

typedef struct Window
{
    struct SDL_Window* _internal;
} Window;

typedef struct WindowResult
{
    Window window;
    b8 valid;
    const char* error_message;
} WindowResult;

WindowResult CreateWindow(i32 width, i32 height, const char* name);
iv2 GetWindowSize(Window window);
b32 GetVulkanExtensions(Window window, u32* count, const char** names);
b32 CreateVulkanSurface(Window window, VkInstance instance, VkSurfaceKHR* surface);
void GetVulkanDrawableSize(Window window, i32* width, i32* height);
const char* GetWindowTitle(Window window);
void DestroyWindow(Window window);
