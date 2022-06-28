#pragma once
#include "colours.h"
#include "window.h"

int VkRendererInit(Window window);
void RendererDrawLine(v3 beg, v3 end, Colour colour);
void VkRendererDraw(void);
void VkRendererShutdown(void);
void VkRendererSetCamera(v3 position, v3 direction);