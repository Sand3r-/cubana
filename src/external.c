#include "external.h"
#include "log/log.h"
#include "errorcodes.h"
#include <SDL.h>

int InitExternalLibs()
{
    if (SDL_Init(0))
    {
        L_ERROR("SDL initialisation failed: %s", SDL_GetError());
        return CU_INITIALIZATION_ERROR;
    }
    L_INFO("SDL Initialised");
    return CU_SUCCESS;
}