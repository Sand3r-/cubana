#pragma once
#include "entity.h"
#include "memory/arena.h"

#define MAX_ENTITIES 32
typedef struct World
{
    Entity entities[MAX_ENTITIES]; // TODO: make it dynamic
    u16    entities_num;
} World;

void WorldInit(Arena* arena, World* world);
void WorldUpdate(World* world, f32 delta, b16 mouse_snap);