#include "world.h"
#include "entity.h"
#include "error.h"
#include "level_loader.h"
#include "math/scalar.h"
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

u32 WorldSpawnEntity(World* world, u32 flags, v3 position, v3 dimensions, v3 colour)
{
    Entity e = {
        .flags = flags,
        .position = position,
        .dimensions = dimensions,
        .colour = colour
    };

    for (u32 i = 0; i < MAX_ENTITIES; i++)
    {
        if (world->entities[i].flags == ENTITY_NONE)
        {
            world->entities[i] = e;
            world->entities_num = max(world->entities_num, i + 1);
            return i;
        }
    }

    ERROR("Failed to spawn entity. MAX_ENTITIES is too small.");
    return -1;
}

void WorldKillEntity(World* world, u32 entity_id)
{
    world->entities[entity_id].flags = ENTITY_NONE;
}

void WorldEntityMove(World* world, u32 entity_id, v3 velocity)
{
    world->entities[entity_id].velocity = velocity;
}

void WorldEntitySetPosition(World* world, u32 entity_id, v3 position)
{
    world->entities[entity_id].position = position;
}

void WorldEntitySetDimensions(World* world, u32 entity_id, v3 dimensions)
{
    world->entities[entity_id].dimensions = dimensions;
}

void WorldEntitySetColour(World* world, u32 entity_id, v3 colour)
{
    world->entities[entity_id].colour = colour;
}

void WorldEntitySetFlags(World* world, u32 entity_id, u32 flags)
{
    world->entities[entity_id].flags = flags;
}

v3 WorldEntityGetPosition(World* world, u32 entity_id)
{
    return world->entities[entity_id].position;
}

v3 WorldEntityGetDimensions(World* world, u32 entity_id)
{
    return world->entities[entity_id].dimensions;
}

v3 WorldEntityGetColour(World* world, u32 entity_id)
{
    return world->entities[entity_id].colour;
}

u32 WorldEntityGetFlags(World* world, u32 entity_id)
{
    return world->entities[entity_id].flags;
}

