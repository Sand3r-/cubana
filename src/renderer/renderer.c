#include "renderer.h"
#include "renderervk.h"

int RendererInit(Arena* arena, Window window)
{
#ifdef USE_VK_RENDERER
    return VkRendererInit(arena, window);
#endif
}

void RendererDraw(Arena* arena, f32 delta)
{
#ifdef USE_VK_RENDERER
    VkRendererDraw(arena, delta);
#endif
}

void RendererShutdown(void)
{
#ifdef USE_VK_RENDERER
    VkRendererShutdown();
#endif
}

void RendererSetCamera(v3 position, v3 direction)
{
#ifdef USE_VK_RENDERER
    VkRendererSetCamera(position, direction);
#endif
}
