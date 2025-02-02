#pragma once
#include "memory/arena.h"
#include "window.h"

int VkRendererInit(Arena* arena, Window window);
void VkRendererBeginFrame(Arena* arena);
void VkRendererRender(Arena* arena, f32 delta);
void VkRendererDrawCube(v3 position, v3 colour);
void VkRendererShutdown(void);
void VkRendererSetCamera(v3 position, v3 direction);