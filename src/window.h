#pragma once
#include "types.h"
#include "math/vec.h"

struct SDL_Window;

typedef struct Window
{
    struct SDL_Window* _internal;
} Window;

typedef struct WindowResult
{
    Window window;
    b8 valid;
    const char* error_message;
} WindowResult;

WindowResult CreateWindow(i32 width, i32 height, const char* name);
iv2 GetWindowSize(Window window);
void DestroyWindow(Window window);
