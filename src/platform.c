#include "platform.h"
#include "gamepad.h"
#include "input.h"
#include "renderer/igintegration.h"
#include <SDL.h>

b8 ProcessPlatformEvents(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0)
    {
        HandleGamepadEvents(e);
        HandleInputEvents(e);
        ImGuiProcessEvents(e);
        if (e.type == SDL_QUIT)
            return 1;
    }
    return 0;
}