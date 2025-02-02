#pragma once
#include "memory/arena.h"
#include "window.h"

int RendererInit(Arena* arena, Window window);
void RendererBeginFrame(Arena* arena);
void RendererRender(Arena* arena, f32 delta);
void RendererDrawCube(v3 position, v3 colour);
void RendererShutdown(void);
void RendererSetCamera(v3 position, v3 direction);