#pragma once
#include "entity.h"
#include "memory/arena.h"

#define MAX_ENTITIES 64
typedef struct World
{
    Entity* current_camera;
    Entity entities[MAX_ENTITIES]; // TODO: make it dynamic
    u32    entities_num;
} World;

void WorldInit(Arena* arena, World* world);
void WorldUpdate(Arena* arena, World* world, f32 delta, b16 edit_mode);
void WorldSetCamera(World* world, u32 entity_id);

u32 WorldSpawnEntity(World* world, Entity entity);
void WorldKillEntity(World* world, u32 entity_id);
void WorldEntityMove(World* world, u32 entity_id, v3 velocity);
void WorldEntitySetName(World* world, u32 entity_id, const char* name);
void WorldEntitySetPosition(World* world, u32 entity_id, v3 position);
void WorldEntitySetRotation(World* world, u32 entity_id, q4 rotation);
void WorldEntitySetDimensions(World* world, u32 entity_id, v3 dimensions);
void WorldEntitySetColour(World* world, u32 entity_id, v3 colour);
void WorldEntitySetFlags(World* world, u32 entity_id, u32 flags);
const char* WorldEntityGetName(World* world, u32 entity_id);
v3 WorldEntityGetPosition(World* world, u32 entity_id);
q4 WorldEntityGetRotation(World* world, u32 entity_id);
v3 WorldEntityGetDimensions(World* world, u32 entity_id);
v3 WorldEntityGetColour(World* world, u32 entity_id);
u32 WorldEntityGetFlags(World* world, u32 entity_id);
void WorldEntityAddChild(Arena* arena, World* world, u32 parent, u32 child);