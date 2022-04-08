#include "window.h"
#include "error.h"
#include <SDL.h>

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

void DestroyWindow(Window window)
{
    SDL_DestroyWindow(window._internal);
}