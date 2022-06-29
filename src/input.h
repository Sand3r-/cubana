#pragma once
#include "types.h"
#include "math/vec.h"

typedef enum
{
    KEY_STATE_UP      = 0x0,
    KEY_STATE_DOWN    = 0x1,
    KEY_STATE_PRESSED = 0x2
} KeyStateBit;


typedef enum
{
    LEFT_ANALOG_STICK = 0,
    RIGHT_ANALOG_STICK = 1
} AnalogStickId;

typedef enum
{
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_SPACE,
    KEY_ENTER,

    KEY_CTRL,
    KEY_SHIFT,
    KEY_ALT,

    KEY_UP,
    KEY_LEFT,
    KEY_DOWN,
    KEY_RIGHT,

    KEY_MAX
} Key;

// Please note that the order in which those buttons appear is utilised by
// HandleGamepadInput function. If the order is changed, please modify it.
typedef enum
{
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_BACK,
    BUTTON_GUIDE,
    BUTTON_START,
    BUTTON_LEFT_STICK,
    BUTTON_RIGHT_STICK,
    BUTTON_LEFT_TRIGGER,
    BUTTON_RIGHT_TRIGGER,
    BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN,
    BUTTON_DPAD_LEFT,
    BUTTON_DPAD_RIGHT,
    BUTTON_LEFT_SHOULDER,
    BUTTON_RIGHT_SHOULDER,

    BUTTON_MAX
} GamePadButton;

// The order stricly corresponds to what SDL is using underneath
typedef enum
{
    BUTTON_LEFT = 1,
    BUTTON_MIDDLE,
    BUTTON_RIGHT,
    BUTTON_X1,
    BUTTON_X2,

    BUTTON_MOUSE_MAX
} Button;

typedef union SDL_Event SDL_Event;

void HandleInputEvents(SDL_Event event);
KeyStateBit GetKeyState(Key key);
KeyStateBit GetButtonState(u8 id, GamePadButton button);
v2 GetAnalogStickState(u8 id, AnalogStickId which);
KeyStateBit GetMouseButtonState(u8 id);
v2 GetMousePosition(void);
v2 GetMouseDelta(void);
void ResetInput(void);
void SnapCursorToCenter(b8 enabled);

void DEBUG_GamepadInput(void);
