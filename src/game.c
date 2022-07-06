#include "game.h"
#include "error.h"
#include "input.h"
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

static void SetSnapCursorToCenter(Game* game, bool enabled)
{
    game->mouse_snap = enabled;
    SnapCursorToCenter(enabled);
}

static void GameControlsUpdate(Game* game)
{
    if (GetKeyState(KEY_F1) & KEY_STATE_PRESSED)
    {
        SetSnapCursorToCenter(game, !game->mouse_snap);
    }
}


int GameInit(Game* game)
{
    SetSnapCursorToCenter(game, true);
    CreateFreeFlyingCamera(game);
    return CU_SUCCESS;
}

void GameUpdate(Game* game, f32 delta)
{
    for (u16 i = 0; i < game->entities_num; i++)
    {
        if (game->entities[i].type & ENTITY_FREE_FLY_CAMERA_BIT)
        {
            if (game->mouse_snap)
            {
                UpdateFreeFlyingCamera(&game->entities[i], delta);
                RendererSetCamera(game->entities[i].position, game->entities[i].direction);
            }
        }
    }

    GameControlsUpdate(game);
}

void GameDestroy(Game* game)
{

}
