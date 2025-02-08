#pragma once
#include "types.h"
#include "math/vec.h"

enum EntityFlags
{
    ENTITY_NONE                = 0,
    ENTITY_PLAYER_BIT          = (1 << 0),
    ENTITY_STATIC_CAMERA_BIT   = (1 << 1),
    ENTITY_ENEMY_BIT           = (1 << 2),
    ENTITY_TRIGGER_BIT         = (1 << 3),
    ENTITY_FREE_FLY_CAMERA_BIT = (1 << 4),
    ENTITY_STATIC_BIT          = (1 << 5),
    ENTITY_COLLIDES_BIT        = (1 << 6),
    ENTITY_VISIBLE_BIT         = (1 << 7),
    ENTITY_GRAVITY_BIT         = (1 << 8)
};

typedef struct Entity
{
    u32 flags;
    v3 position;
    v3 dimensions;
    v3 colour;
    v3 velocity;
    v3 direction;

    union euler
    {
        v3 angles;
        struct
        {
            f32 yaw;
            f32 pitch;
            f32 roll;
        };
    } rotation;
} Entity;

Entity CreateStaticEntity(v3 pos, v3 dimensions, v3 colour);
Entity CreatePlayerEntity(v3 pos, v3 dimensions, v3 colour);
Entity CreateEnemyEntity(v3 pos, v3 dimensions, v3 colour);
Entity CreateFreeFlyingCameraEntity(v3 pos);
void UpdateFreeFlyingCamera(Entity* camera, f32 delta);
void UpdatePlayer(Entity* player);
