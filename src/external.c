#include "external.h"
#include "log/log.h"
#include "error.h"
#include <SDL.h>

int InitExternalLibs()
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        L_ERROR("SDL initialisation failed: %s", SDL_GetError());
        return CU_INITIALIZATION_ERROR;
    }
    L_INFO("SDL Initialised");
    return CU_SUCCESS;
}