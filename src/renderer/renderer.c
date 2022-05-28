#include "renderer.h"
#include "renderervk.h"

int RendererInit(Window window)
{
#ifdef USE_VK_RENDERER
    return VkRendererInit(window);
#endif
}

void RendererDraw(void)
{
#ifdef USE_VK_RENDERER
    VkRendererDraw();
#endif
}

void RendererShutdown(void)
{
#ifdef USE_VK_RENDERER
    VkRendererShutdown();
#endif
}
