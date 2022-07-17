#pragma once
#include "entity.h"

#define MAX_ENTITIES 8

typedef union Event Event;

typedef struct Game
{
    Entity entities[MAX_ENTITIES];
    u16    entities_num;
    b16    mouse_snap;
} Game;

int GameInit(Game* game);
void GameUpdate(Game* game, f32 delta);
void GameProcessEvent(Game* game, Event event);
void GameDestroy(Game* game);
