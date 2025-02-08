#include "world.h"
#include "entity.h"
#include "error.h"
#include "level_loader.h"
#include "memory/thread_scratch.h"
#include "physics.h"

static void CreateFreeFlyingCamera(World* world)
{
    // Check for existing free flying camera
    for (u16 i = 0; i < world->entities_num; i++)
    {
        if (world->entities[i].flags & ENTITY_FREE_FLY_CAMERA_BIT)
            ERROR("An existing free flying camera already exists.");
    }

    // Create a camera
    v3 position = v3(2.0f, 2.0f, 2.0f);
    world->entities[world->entities_num++] = CreateFreeFlyingCameraEntity(position);
}

static void WorldLoadLevel(Arena* arena, World* world, char* path)
{
    Arena* scratch = GetScratchArena();
    with_arena(scratch)
    {
        LevelData* level_data = LoadTextLevelData(scratch, "FirstMap.ctm");
        for (u16 i = 0; i < level_data->count; i++)
        {
            ObjectData* obj = &level_data->objects[i];
            world->entities[world->entities_num++] =
                CreateStaticEntity(obj->position, obj->dimensions, obj->colour);
        }
    }

    // Add example player and enemy
    Entity player = CreatePlayerEntity(v3(0.0f, 10.0f, 0.0f), v3(1.0f, 1.0f, 1.0f), v3(0.64f, 1.f, 1.f));
    world->entities[world->entities_num++] = player;

    Entity enemy = CreateEnemyEntity(v3(3.0f, 10.0f, 0.0f), v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f));
    world->entities[world->entities_num++] = enemy;

}

void WorldInit(Arena* arena, World* world)
{
    CreateFreeFlyingCamera(world);
    WorldLoadLevel(arena, world, "assets/TestLevel.cmt");
}

void WorldUpdate(Arena* arena, World* world, f32 delta, b16 mouse_snap)
{
    for (u16 i = 0; i < world->entities_num; i++)
    {
        if (world->entities[i].flags & ENTITY_PLAYER_BIT)
        {
            UpdatePlayer(&world->entities[i]);
        }
        else if (world->entities[i].flags & ENTITY_FREE_FLY_CAMERA_BIT)
        {
            if (mouse_snap)
            {
                UpdateFreeFlyingCamera(&world->entities[i], delta);
                RendererSetCamera(world->entities[i].position, world->entities[i].direction);
            }
        }

        if (world->entities[i].flags & ENTITY_VISIBLE_BIT)
        {
            v3 position = world->entities[i].position;
            v3 dimensions = world->entities[i].dimensions;
            v3 colour = world->entities[i].colour;
            RendererDrawCube(position, dimensions, colour);
        }
    }
    PhysicsUpdate(arena, world->entities, world->entities_num, delta);
}