#pragma once
#include "entity.h"
#include "memory/arena.h"

#define MAX_ENTITIES 32
typedef struct World
{
    Entity* current_camera;
    Entity entities[MAX_ENTITIES]; // TODO: make it dynamic
    u32    entities_num;
} World;

void WorldInit(Arena* arena, World* world);
void WorldUpdate(Arena* arena, World* world, f32 delta, b16 mouse_snap);

u32 WorldSpawnEntity(World* world, u32 flags, v3 position, v3 dimensions, v3 colour);
void WorldKillEntity(World* world, u32 entity_id);
void WorldEntityMove(World* world, u32 entity_id, v3 velocity);
void WorldEntitySetPosition(World* world, u32 entity_id, v3 position);
void WorldEntitySetDimensions(World* world, u32 entity_id, v3 dimensions);
void WorldEntitySetColour(World* world, u32 entity_id, v3 colour);
void WorldEntitySetFlags(World* world, u32 entity_id, u32 flags);
v3 WorldEntityGetPosition(World* world, u32 entity_id);
v3 WorldEntityGetDimensions(World* world, u32 entity_id);
v3 WorldEntityGetColour(World* world, u32 entity_id);
u32 WorldEntityGetFlags(World* world, u32 entity_id);