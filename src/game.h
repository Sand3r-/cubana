#pragma once
#include "entity.h"

#define MAX_ENTITIES 8

typedef struct Game
{
    Entity entities[MAX_ENTITIES];
    u16    entities_num;
} Game;

int GameInit(Game* game);
void GameUpdate(Game* game);
void GameDestroy(Game* game);
