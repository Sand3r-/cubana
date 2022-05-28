#pragma once
#include "types.h"

#define MAX_GAMEPADS_NUM 2

typedef union SDL_Event SDL_Event;

void HandleGamepadEvents(SDL_Event event);
u8 GetGamepadSlot(s32 joystick_id);
void CloseGamepadControllers(void);