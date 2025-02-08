#include "collision.h"
#include "math/scalar.h"

CollisionResult CheckCollision(Entity *a, Entity *b)
{
    CollisionResult result = {0};
    if (a->flags & ENTITY_COLLIDES_BIT && b->flags & ENTITY_COLLIDES_BIT)
    {
        // Based on Chirster's Ericson Real-Time Collision Detection
        // Center-radius AABB Intersection Test algorithm
        v3 diff = { abs(a->position.x - b->position.x),
                    abs(a->position.y - b->position.y),
                    abs(a->position.z - b->position.z) };

        v3 extents_sum = { (a->dimensions.x + b->dimensions.x) * 0.5f,
                           (a->dimensions.y + b->dimensions.y) * 0.5f,
                           (a->dimensions.z + b->dimensions.z) * 0.5f };

        if (diff.x < extents_sum.x && diff.y < extents_sum.y && diff.z < extents_sum.z)
        {
            result.collided = 1;

            // Compute penetration depth for each axis
            v3 penetration = { extents_sum.x - diff.x,
                               extents_sum.y - diff.y,
                               extents_sum.z - diff.z };

             // Select the axis with the smallest penetration depth
            float min_penetration = min(penetration.x, min(penetration.y, penetration.z));

            // Assign penetration vector along the smallest axis
            result.penetration_vec.x = (min_penetration == penetration.x) ? copysign(penetration.x, a->position.x - b->position.x) : 0.0f;
            result.penetration_vec.y = (min_penetration == penetration.y) ? copysign(penetration.y, a->position.y - b->position.y) : 0.0f;
            result.penetration_vec.z = (min_penetration == penetration.z) ? copysign(penetration.z, a->position.z - b->position.z) : 0.0f;
        }
    }

    return result;
}