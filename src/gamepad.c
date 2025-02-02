#include "gamepad.h"
#include "log/log.h"
#include "math/scalar.h"
#include <SDL_gamecontroller.h>
#include <SDL_events.h>

static struct
{
    SDL_GameController* controller[MAX_GAMEPADS_NUM];
    s32 ids[MAX_GAMEPADS_NUM];
    u8 num_gamepads_used;
} G;

static s32 GetJoyId(SDL_GameController* c)
{
    SDL_Joystick* j = SDL_GameControllerGetJoystick(c);
    return SDL_JoystickInstanceID(j);
}

void CloseGamepadControllers(void)
{
    for (u8 i = 0; i < MAX_GAMEPADS_NUM; i++)
        if (G.controller[i] != NULL)
            SDL_GameControllerClose(G.controller[i]);
}

static inline void HandleGamepadConnected(SDL_Event event)
{
    if (++G.num_gamepads_used > MAX_GAMEPADS_NUM)
    {
        L_WARN("Attempt to add new gamepad, but there are total of (%d) \
                supported", MAX_GAMEPADS_NUM);
        return;
    }
    u8 device_index = (u8)event.cdevice.which;
    u8 slot = G.controller[0] == NULL ? 0 : 1;

    G.controller[slot] = SDL_GameControllerOpen(device_index);
    G.ids[slot] = GetJoyId(G.controller[slot]);

    L_INFO("Controller %d connected, with joystick id %d", slot, G.ids[slot]);
}

static inline void HandleGamepadDisconnected(SDL_Event event)
{
    u8 slot = GetGamepadSlot(event.cdevice.which);
    SDL_GameControllerClose(G.controller[slot]);
    G.controller[slot] = NULL;
    G.ids[slot] = -1;
    G.num_gamepads_used--;
    L_INFO("Controller %d with id %d removed", slot, event.cdevice.which);
}

void HandleGamepadEvents(SDL_Event event)
{
    if (event.type == SDL_CONTROLLERDEVICEADDED)
        HandleGamepadConnected(event);

    if (event.type == SDL_CONTROLLERDEVICEREMOVED)
        HandleGamepadDisconnected(event);
}

u8 GetGamepadSlot(s32 joystick_id)
{
    return joystick_id == G.ids[0] ? 0 : 1;
}