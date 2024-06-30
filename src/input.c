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

typedef struct MouseState
{
    Button buttons[BUTTON_MOUSE_MAX];
    v2 position;
    v2 delta;
    b8 snap;
} MouseState;

static struct
{
    u8 keyboard[KEY_MAX];
    GamePadState gamepad[MAX_GAMEPADS_NUM];
    MouseState mouse;
} IO;


static void HandleKeyboardInput(SDL_Event event)
{
#define MAP_SDL_BINDING(sdl_scancode, key, pressed) case sdl_scancode: \
    IO.keyboard[key] = pressed; \
    break;

    // This game will use scan codes, since I'll most likely only use
    // WASD and surrounding keys for input + the function keys for developer
    // purposes only.
    u8 flags = event.type == SDL_KEYDOWN ? KEY_STATE_PRESSED | KEY_STATE_DOWN : KEY_STATE_UP;
    switch(event.key.keysym.scancode)
    {
        MAP_SDL_BINDING(SDL_SCANCODE_F1, KEY_F1, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F2, KEY_F2, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F3, KEY_F3, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F4, KEY_F4, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F5, KEY_F5, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F6, KEY_F6, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F7, KEY_F7, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F8, KEY_F8, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F9, KEY_F9, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F10, KEY_F10, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F11, KEY_F11, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F12, KEY_F12, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_A, KEY_A, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_B, KEY_B, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_C, KEY_C, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_D, KEY_D, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_E, KEY_E, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_F, KEY_F, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_G, KEY_G, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_H, KEY_H, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_I, KEY_I, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_J, KEY_J, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_K, KEY_K, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_L, KEY_L, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_M, KEY_M, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_N, KEY_N, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_O, KEY_O, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_P, KEY_P, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_Q, KEY_Q, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_R, KEY_R, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_S, KEY_S, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_T, KEY_T, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_U, KEY_U, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_V, KEY_V, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_W, KEY_W, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_X, KEY_X, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_Y, KEY_Y, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_Z, KEY_Z, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_ESCAPE, KEY_ESC, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_RETURN, KEY_ENTER, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_SPACE, KEY_SPACE, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_LCTRL, KEY_CTRL, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_LSHIFT, KEY_SHIFT, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_LALT, KEY_ALT, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_RIGHT, KEY_RIGHT, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_LEFT, KEY_LEFT, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_DOWN, KEY_DOWN, flags);
        MAP_SDL_BINDING(SDL_SCANCODE_UP, KEY_UP, flags);
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

static void HandleGamepadAxis(SDL_Event event)
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

static void HandleGamepadInput(SDL_Event event)
{
    u8 slot = GetGamepadSlot(event.caxis.which);
    int button = event.cbutton.button;
    b8 was_pressed = event.type == SDL_CONTROLLERBUTTONDOWN;
    if (button >= 0 && button < BUTTON_MAX)
        IO.gamepad[slot].buttons[button] = was_pressed;
}

static void HandleMouseMove(SDL_Event event)
{
    // TODO(Sand3r-): Notice that normalisation causes it to be assymetrical
    // in terms of input handling. consider dividing by min(width, height).
    SDL_Window* window = SDL_GetWindowFromID(event.motion.windowID);
    iv2 size;
    SDL_GetWindowSize(window, &size.width, &size.height);
    if (IO.mouse.snap)
    {
        const f32 center_x = size.width  / 2;
        const f32 center_y = size.height / 2;

        IO.mouse.delta = v2(-(center_x - event.motion.x) / (f32) size.width,
                             (center_y - event.motion.y) / (f32) size.height);
        SDL_WarpMouseInWindow(window, center_x, center_y);
    }
    else
    {
        IO.mouse.position = v2(event.motion.x, event.motion.y);
    }
}

static void HandleMouseInput(SDL_Event event)
{
    int button = (i32)event.button.button - 1;
    b8 was_pressed = event.type == SDL_MOUSEBUTTONDOWN;
    if (button >= 0 && button < BUTTON_MOUSE_MAX)
        IO.mouse.buttons[button] = was_pressed;
}

void HandleInputEvents(SDL_Event event)
{
    if (event.type == SDL_MOUSEMOTION)
        HandleMouseMove(event);

    if (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
        HandleMouseInput(event);

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        HandleKeyboardInput(event);

    if (event.type == SDL_CONTROLLERAXISMOTION)
        HandleGamepadAxis(event);

    if (event.type == SDL_CONTROLLERBUTTONDOWN ||
        event.type == SDL_CONTROLLERBUTTONUP)
        HandleGamepadInput(event);

}

KeyStateBit GetKeyState(Key key)
{
    return IO.keyboard[key];
}

KeyStateBit GetButtonState(u8 id, GamePadButton button)
{
    return IO.gamepad[id].buttons[button];
}

v2 GetAnalogStickState(u8 id, AnalogStickId which)
{
    return IO.gamepad[id].analog[which];
}

KeyStateBit GetMouseButtonState(u8 id)
{
    return IO.mouse.buttons[id - 1];
}

v2 GetMousePosition(void)
{
    return IO.mouse.position;
}

v2 GetMouseDelta(void)
{
    return IO.mouse.delta;
}

void SnapCursorToCenter(b8 enabled)
{
    IO.mouse.snap = enabled;
    SDL_SetRelativeMouseMode(enabled);
}

void ResetInput(void)
{
    IO.mouse.delta = v2(0.0f);
    for (u32 i = 0; i < KEY_MAX; i++)
    {
        IO.keyboard[i] &= KEY_STATE_DOWN;
    }
}

void DEBUG_MouseMotion(void)
{

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
