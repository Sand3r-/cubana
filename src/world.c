#include "world.h"
#include "entity.h"
#include "error.h"
#include "level_loader.h"
#include "memory/thread_scratch.h"

static void CreateFreeFlyingCamera(World* world)
{
    // Check for existing free flying camera
    for (u16 i = 0; i < world->entities_num; i++)
    {
        if (world->entities[i].type & ENTITY_FREE_FLY_CAMERA_BIT)
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
}

void WorldInit(Arena* arena, World* world)
{
    CreateFreeFlyingCamera(world);
    WorldLoadLevel(arena, world, "FirstMap.cmt");
}

void WorldUpdate(World* world, f32 delta, b16 mouse_snap)
{
    for (u16 i = 0; i < world->entities_num; i++)
    {
        if (world->entities[i].type & ENTITY_FREE_FLY_CAMERA_BIT)
        {
            if (mouse_snap)
            {
                UpdateFreeFlyingCamera(&world->entities[i], delta);
                RendererSetCamera(world->entities[i].position, world->entities[i].direction);
            }
        }
        else if (world->entities[i].type & ENTITY_STATIC_BIT)
        {
            v3 position = world->entities[i].position;
            v3 dimensions = world->entities[i].dimensions;
            v3 colour = world->entities[i].colour;
            RendererDrawCube(position, dimensions, colour);
        }
    }

}