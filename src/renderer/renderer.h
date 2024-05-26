#pragma once
#include "window.h"

int RendererInit(Window window);
void RendererDraw(f32 delta);
void RendererShutdown(void);
void RendererSetCamera(v3 position, v3 direction);