#include "game.h"
#include "error.h"
#include "renderer/renderer.h"

static void CreateFreeFlyingCamera(Game* game)
{
    // Check for existing free flying camera
    for (u16 i = 0; i < game->entities_num; i++)
    {
        if (game->entities[i].type & ENTITY_FREE_FLY_CAMERA_BIT)
            ERROR("An existing free flying camera already exists.");
    }

    // Create a camera
    v3 position = v3(2.0f, 2.0f, 2.0f);
    game->entities[game->entities_num++] = CreateFreeFlyingCameraEntity(position);
}

int GameInit(Game* game)
{
    CreateFreeFlyingCamera(game);
    return CU_SUCCESS;
}

void GameUpdate(Game* game)
{
    for (u16 i = 0; i < game->entities_num; i++)
    {
        if (game->entities[i].type & ENTITY_FREE_FLY_CAMERA_BIT)
        {
            UpdateFreeFlyingCamera(&game->entities[i]);
            RendererSetCamera(game->entities[i].position, game->entities[i].direction);
        }
    }

}

void GameDestroy(Game* game)
{

}
