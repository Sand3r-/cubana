#pragma once
#include "types.h"

// Warning: changing this number won't just magically allow more controllers
// to work. Code makes some heavy assumptions that there are max 2 controllers.
#define MAX_GAMEPADS_NUM 2

typedef union SDL_Event SDL_Event;

void HandleGamepadEvents(SDL_Event event);
u8 GetGamepadSlot(s32 joystick_id);
void CloseGamepadControllers(void);