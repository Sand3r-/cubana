#pragma once
#include "colours.h"
#include "window.h"

int RendererInit(Window window);
void RendererDrawLine(v3 beg, v3 end, Colour colour);
void RendererDraw(void);
void RendererShutdown(void);
void RendererSetCamera(v3 position, v3 direction);