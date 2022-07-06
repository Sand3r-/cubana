#include "timer.h"
#include <SDL_timer.h>

u64 GetTicks(void)
{
    return SDL_GetTicks64();
}

u64 GetPerformanceCounter(void)
{
    return SDL_GetPerformanceCounter();
}

u64 GetPerformanceFrequency(void)
{
    return SDL_GetPerformanceFrequency();
}
