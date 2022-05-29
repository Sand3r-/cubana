#pragma once
#include "window.h"

int VkRendererInit(Window window);
void VkRendererDraw(void);
void VkRendererShutdown(void);
void VkRendererSetCamera(v3 position, v3 direction);