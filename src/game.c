#include "game.h"
#include "error.h"
#include "event.h"
#include "input.h"
#include "log/log.h"

#include "renderer/renderer.h"
#include "script/scripting.h"
#include "ui/ui.h"



static void EnableEditMode(Game* game, bool enabled)
{
    game->edit_mode = enabled;
    SnapCursorToCenter(!enabled);
}

static void GameControlsUpdate(Game* game)
{
    if (GetKeyState(KEY_ESC) & KEY_STATE_PRESSED)
    {
        EnableEditMode(game, !game->edit_mode);
    }
}

int GameInit(Arena* arena, Game* game)
{
    InitUI(arena);
    EnableEditMode(game, false);
    WorldInit(arena, &game->world);
    LuaWorldSetCurrentWorld(&game->world);
    LuaWorldSetCurrentArena(arena);
    ExecuteScriptFile("scripts/level1.lua");
    ExecuteScriptFile("scripts/player.lua");
    ExecuteScriptFile("scripts/enemy.lua");
    EmitEvent(CreateEventGameBegin());
    return CU_SUCCESS;
}

void GameUpdate(Arena* arena, Game* game, f32 delta)
{
    WorldUpdate(arena, &game->world, delta, game->edit_mode);
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
