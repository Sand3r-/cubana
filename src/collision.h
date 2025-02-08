#pragma once
#include "entity.h"

typedef struct CollisionResult
{
    v3 penetration_vec;
    b32 collided;
} CollisionResult;

CollisionResult CheckCollision(Entity* a, Entity* b);