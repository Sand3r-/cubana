#include "renderer.h"
#include "renderervk.h"

int RendererInit(Arena* arena, Window window)
{
#ifdef USE_VK_RENDERER
    return VkRendererInit(arena, window);
#endif
}

void RendererBeginFrame(Arena* arena)
{
#ifdef USE_VK_RENDERER
    VkRendererBeginFrame(arena);
#endif
}

void RendererRender(Arena* arena, f32 delta)
{
#ifdef USE_VK_RENDERER
    VkRendererRender(arena, delta);
#endif
}

void RendererDrawCube(v3 position, v3 colour)
{
#ifdef USE_VK_RENDERER
    VkRendererDrawCube(position, colour);
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
