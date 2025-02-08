#include "game.h"
#include "error.h"
#include "event.h"
#include "input.h"
#include "log/log.h"

#include "renderer/renderer.h"
#include "script/scripting.h"
#include "ui/ui.h"



static void SetSnapCursorToCenter(Game* game, bool enabled)
{
    game->mouse_snap = enabled;
    SnapCursorToCenter(enabled);
}

static void GameControlsUpdate(Game* game)
{
    if (GetKeyState(KEY_ESC) & KEY_STATE_PRESSED)
    {
        SetSnapCursorToCenter(game, !game->mouse_snap);
    }
}

int GameInit(Arena* arena, Game* game)
{
    InitUI(arena);
    SetSnapCursorToCenter(game, true);
    WorldInit(arena, &game->world);
    ExecuteScriptFile("scripts/level1.lua");
    EmitEvent(CreateEventGameBegin());
    return CU_SUCCESS;
}

void GameUpdate(Arena* arena, Game* game, f32 delta)
{
    WorldUpdate(arena, &game->world, delta, game->mouse_snap);
    GameControlsUpdate(game);
    UpdateUI();
}

void GameProcessEvent(Game* game, Event event)
{
    switch (event.type)
    {
        case EVENT_GAME_BEGIN:
            L_INFO("Oh yes, and so the game begun");
            break;
        default:
            break;
    }
    // mmm yes consume the events
}

void GameDestroy(Game* game)
{
    EmitEvent(CreateEventGameEnd());
}
