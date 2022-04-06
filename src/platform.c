#include "platform.h"
#include <SDL.h>

b8 ProcessPlatformEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0)
    {
        if (e.type == SDL_QUIT)
            return 1;
    }
    return 0;
}