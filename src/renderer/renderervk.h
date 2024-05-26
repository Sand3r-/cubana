#pragma once
#include "window.h"

int VkRendererInit(Window window);
void VkRendererDraw(f32 delta);
void VkRendererShutdown(void);
void VkRendererSetCamera(v3 position, v3 direction);