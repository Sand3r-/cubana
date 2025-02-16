#pragma once
#include "types.h"
#include "math/vec.h"
#include "math/mat.h"
#include "math/qat.h"
#include "memory/arena.h"

#define ENTITY_MAX_CHILDREN_NUM 4

enum EntityFlags
{
    ENTITY_NONE                = 0,
    ENTITY_PLAYER_BIT          = (1 << 0),
    ENTITY_CAMERA_BIT          = (1 << 1),
    ENTITY_ENEMY_BIT           = (1 << 2),
    ENTITY_TRIGGER_BIT         = (1 << 3),
    ENTITY_FREE_FLY_CAMERA_BIT = (1 << 4),
    ENTITY_STATIC_BIT          = (1 << 5),
    ENTITY_COLLIDES_BIT        = (1 << 6),
    ENTITY_VISIBLE_BIT         = (1 << 7),
    ENTITY_GRAVITY_BIT         = (1 << 8)
};

typedef struct Entity Entity;

typedef struct Entity
{
    const char* name;
    u32 flags;
    v3 position;
    q4 rotation;
    v3 dimensions;
    v3 colour;
    v3 velocity;

    Entity* parent;
    Entity* children;
    u32 children_num;
} Entity;

const char* EntityGetName(Entity* entity);
void EntityAddChild(Arena* arena, Entity* entity, Entity* child);
m4 EntityGetWorldTransform(Entity* entity);
m4 EntityGetLocalTransform(Entity* entity);

Entity CreateStaticEntity(const char* name, v3 pos, v3 dimensions, v3 colour);
Entity CreatePlayerEntity(v3 pos, v3 dimensions, v3 colour);
Entity CreateEnemyEntity(const char* name, v3 pos, v3 dimensions, v3 colour);
Entity CreateFreeFlyingCameraEntity(v3 pos);
Entity CreateCameraEntity(const char* name, v3 pos, v3 vel, q4 rot);

void UpdateFreeFlyingCamera(Entity* camera, f32 delta);
void UpdatePlayer(Entity* player);
