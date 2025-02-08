#include "physics.h"
#include "collision.h"

void PhysicsUpdate(Arena* arena, Entity* entities, u32 entities_num, f32 delta)
{
    with_arena(arena)
    {
        u32* dynamic_entities_indices = PushArray(arena, u32, entities_num);
        u32 dynamic_entities_num = 0;
        for (u32 i = 0; i < entities_num; i++)
        {
            if (entities[i].flags & ENTITY_GRAVITY_BIT)
            {
                entities[i].velocity.y = -.0098f;
            }
            if (entities[i].flags & (ENTITY_PLAYER_BIT | ENTITY_ENEMY_BIT))
            {
                entities[i].position.x += entities[i].velocity.x * delta;
                entities[i].position.y += entities[i].velocity.y * delta;
                entities[i].position.z += entities[i].velocity.z * delta;
                dynamic_entities_indices[dynamic_entities_num++] = i;
            }
        }

        for (u32 i = 0; i < dynamic_entities_num; i++)
        {
            u32 d = dynamic_entities_indices[i];
            for (u32 e = 0; e < entities_num; e++)
            {
                if (d == e)
                    continue;

                CollisionResult result = CheckCollision(&entities[d], &entities[e]);
                if (result.collided)
                {
                    if (entities[e].flags & ENTITY_STATIC_BIT)
                    {
                        entities[d].position.x += result.penetration_vec.x;
                        entities[d].position.y += result.penetration_vec.y;
                        entities[d].position.z += result.penetration_vec.z;
                    }
                    else if (entities[d].flags & (ENTITY_ENEMY_BIT | ENTITY_PLAYER_BIT))
                    {
                        entities[d].position.x += result.penetration_vec.x * 0.5f;
                        entities[d].position.y += result.penetration_vec.y * 0.5f;
                        entities[d].position.z += result.penetration_vec.z * 0.5f;
                    }
                }
            }
        }
    }
}
