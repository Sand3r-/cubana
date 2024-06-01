#pragma once
#include "memory/arena.h"
#include "window.h"

int RendererInit(Arena* arena, Window window);
void RendererDraw(Arena* arena, f32 delta);
void RendererShutdown(void);
void RendererSetCamera(v3 position, v3 direction);