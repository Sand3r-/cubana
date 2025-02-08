#pragma once
#include "memory/arena.h"
#include "entity.h"

void PhysicsUpdate(Arena* arena, Entity* entities, u32 entities_num, f32 delta);