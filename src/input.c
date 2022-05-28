#include "input.h"
#include "gamepad.h"
#include "log/log.h"
#include <SDL_events.h>

#define ANALOGSTICK_DEAD_ZONE 8000

typedef v2 GamePadAnalogStickState;

typedef struct GamePadState
{
    GamePadAnalogStickState analog[2];
    GamePadButton buttons[BUTTON_MAX];
} GamePadState;

static struct
{
    Key keyboard[KEY_MAX];
    GamePadState gamepad[MAX_GAMEPADS_NUM];
} IO;


static inline void HandleKeyboardInput(SDL_Event event)
{
#define MAP_SDL_BINDING(sdl_scancode, key, pressed) case sdl_scancode: \
    IO.keyboard[key] = pressed; \
    break;

    // This game will use scan codes, since I'll most likely only use
    // WASD and surrounding keys for input + the function keys for developer
    // purposes only.
    b8 was_pressed = event.type == SDL_KEYDOWN;
    switch(event.key.keysym.scancode)
    {
        MAP_SDL_BINDING(SDL_SCANCODE_F1, KEY_F1, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F2, KEY_F2, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F3, KEY_F3, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F4, KEY_F4, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F5, KEY_F5, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F6, KEY_F6, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F7, KEY_F7, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F8, KEY_F8, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F9, KEY_F9, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F10, KEY_F10, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F11, KEY_F11, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F12, KEY_F12, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_A, KEY_A, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_B, KEY_B, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_C, KEY_C, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_D, KEY_D, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_E, KEY_E, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_F, KEY_F, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_G, KEY_G, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_H, KEY_H, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_I, KEY_I, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_J, KEY_J, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_K, KEY_K, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_L, KEY_L, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_M, KEY_M, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_N, KEY_N, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_O, KEY_O, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_P, KEY_P, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_Q, KEY_Q, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_R, KEY_R, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_S, KEY_S, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_T, KEY_T, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_U, KEY_U, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_V, KEY_V, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_W, KEY_W, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_X, KEY_X, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_Y, KEY_Y, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_Z, KEY_Z, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_RETURN, KEY_ENTER, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_SPACE, KEY_SPACE, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_LCTRL, KEY_CTRL, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_LSHIFT, KEY_SHIFT, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_LALT, KEY_ALT, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_RIGHT, KEY_RIGHT, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_LEFT, KEY_LEFT, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_DOWN, KEY_DOWN, was_pressed);
        MAP_SDL_BINDING(SDL_SCANCODE_UP, KEY_UP, was_pressed);
    }
#undef MAP_SDL_BINDING
}

static inline f32 NormalizeAxis(int value)
{
    if (value > ANALOGSTICK_DEAD_ZONE)
        return (f32)value / SDL_JOYSTICK_AXIS_MAX;

    if (value < -ANALOGSTICK_DEAD_ZONE)
        return -((f32)value / SDL_JOYSTICK_AXIS_MIN);

    return 0.0f;
}

static inline void HandleGamepadAxis(SDL_Event event)
{
    u8 slot = GetGamepadSlot(event.caxis.which);
    switch (event.caxis.axis)
    {
        case SDL_CONTROLLER_AXIS_LEFTX:
            IO.gamepad[slot].analog[LEFT_ANALOG_STICK].x =
                NormalizeAxis(event.caxis.value);
            break;
        case SDL_CONTROLLER_AXIS_LEFTY:
            IO.gamepad[slot].analog[LEFT_ANALOG_STICK].y =
                NormalizeAxis(event.caxis.value);
            break;
        case SDL_CONTROLLER_AXIS_RIGHTX:
            IO.gamepad[slot].analog[RIGHT_ANALOG_STICK].x =
                NormalizeAxis(event.caxis.value);
            break;
        case SDL_CONTROLLER_AXIS_RIGHTY:
            IO.gamepad[slot].analog[RIGHT_ANALOG_STICK].y =
                NormalizeAxis(event.caxis.value);
            break;
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            IO.gamepad[slot].buttons[BUTTON_LEFT_TRIGGER] =
                event.caxis.value > SDL_JOYSTICK_AXIS_MAX / 2;
            break;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            IO.gamepad[slot].buttons[BUTTON_RIGHT_TRIGGER] =
                event.caxis.value > SDL_JOYSTICK_AXIS_MAX / 2;
            break;
    }
}

static inline void HandleGamepadInput(SDL_Event event)
{
    u8 slot = GetGamepadSlot(event.caxis.which);
    int button = event.cbutton.button;
    b8 was_pressed = event.type == SDL_CONTROLLERBUTTONDOWN;
    if (button >= 0 && button < BUTTON_MAX)
        IO.gamepad[slot].buttons[button] = was_pressed;
}

void HandleInputEvents(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        HandleKeyboardInput(event);

    if (event.type == SDL_CONTROLLERAXISMOTION)
        HandleGamepadAxis(event);

    if (event.type == SDL_CONTROLLERBUTTONDOWN ||
        event.type == SDL_CONTROLLERBUTTONUP)
        HandleGamepadInput(event);
}

KeyState GetKeyState(Key key)
{
    return IO.keyboard[key];
}

KeyState GetButtonState(u8 id, GamePadButton button)
{
    return IO.gamepad[id].buttons[button];
}

v2 GetAnalogStickState(u8 id, AnalogStickId which)
{
    return IO.gamepad[id].analog[which];
}

void DEBUG_GamepadInput(void)
{
    const char* names[] = {
        "BUTTON_A",
        "BUTTON_B",
        "BUTTON_X",
        "BUTTON_Y",
        "BUTTON_BACK",
        "BUTTON_GUIDE",
        "BUTTON_START",
        "BUTTON_LEFT_STICK",
        "BUTTON_RIGHT_STICK",
        "BUTTON_LEFT_TRIGGER",
        "BUTTON_RIGHT_TRIGGER",
        "BUTTON_DPAD_UP",
        "BUTTON_DPAD_DOWN",
        "BUTTON_DPAD_LEFT",
        "BUTTON_DPAD_RIGHT",
        "BUTTON_LEFT_SHOULDER",
        "BUTTON_RIGHT_SHOULDER",
    };

    for (u8 i = 0; i < ArrayCount(names); i++)
        for (u8 j = 0; j < MAX_GAMEPADS_NUM; j++)
            if (GetButtonState(j, i))
                L_INFO("Button %s pressed on %d", names[i], j);

    for (u8 j = 0; j < MAX_GAMEPADS_NUM; j++)
    {
        v2 left_stick = GetAnalogStickState(j, LEFT_ANALOG_STICK);
        if (left_stick.x != 0.0f)
            L_INFO("Left stick x: %f", left_stick.x);
        if (left_stick.y != 0.0f)
            L_INFO("Left stick y: %f", left_stick.y);

        v2 right_stick = GetAnalogStickState(j, RIGHT_ANALOG_STICK);
        if (right_stick.x != 0.0f)
            L_INFO("Right stick x: %f", right_stick.x);
        if (right_stick.y != 0.0f)
            L_INFO("Right stick y: %f", right_stick.y);
    }
}
