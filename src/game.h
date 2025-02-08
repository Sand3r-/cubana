#pragma once
#include "memory/arena.h"
#include "world.h"

typedef union Event Event;

typedef struct Game
{
    World world;
    b16    mouse_snap;
} Game;

int GameInit(Arena* arena, Game* game);
void GameUpdate(Arena* arena, Game* game, f32 delta);
void GameProcessEvent(Game* game, Event event);
void GameDestroy(Game* game);
