#pragma once
#include "memory/arena.h"
#include "window.h"

int VkRendererInit(Arena* arena, Window window);
void VkRendererDraw(Arena* arena, f32 delta);
void VkRendererShutdown(void);
void VkRendererSetCamera(v3 position, v3 direction);