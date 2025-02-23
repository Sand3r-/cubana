#include "world.h"
#include "culibc.h"
#include "entity.h"
#include "error.h"
#include "level_loader.h"
#include "math/mat.h"
#include "math/scalar.h"
#include "memory/thread_scratch.h"
#include "physics.h"
#include "cimgui.h"

static Entity* WorldAddEntity(World* world, Entity entity)
{
    assert(world->entities_num < MAX_ENTITIES);
    world->entities[world->entities_num] = entity;
    return &world->entities[world->entities_num++];
}

static char* PushString(Arena* arena, char* source)
{
    size_t source_len = cu_strlen(source);
    char* dest = PushArray(arena, char, source_len + 1);
    cu_memcpy(dest, source, source_len);
    return dest;
}

static Entity* CreateFreeFlyingCamera(World* world)
{
    // Check for existing free flying camera
    for (u16 i = 0; i < world->entities_num; i++)
    {
        if (world->entities[i].flags & ENTITY_FREE_FLY_CAMERA_BIT)
            ERROR("An existing free flying camera already exists.");
    }

    // Create a camera
    v3 position = v3(2.0f, 2.0f, 2.0f);
    return WorldAddEntity(world, CreateFreeFlyingCameraEntity(position));
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
            char* name = PushString(arena, obj->name);
            Entity entity = CreateStaticEntity(name, obj->position, obj->dimensions, obj->colour);
            WorldAddEntity(world, entity);
        }
    }
}

void WorldInit(Arena* arena, World* world)
{
    WorldLoadLevel(arena, world, "assets/TestLevel.cmt");
    Entity* camera = CreateFreeFlyingCamera(world);
    world->current_camera = camera;
}

void WorldSetCamera(World* world, u32 entity_id)
{
    Entity* camera = &world->entities[entity_id];
    world->current_camera = camera;
}

static void RenderEntity(Entity* entity)
{
    v3 position = PositionFromTransform(EntityGetWorldTransform(entity));
    v3 dimensions = entity->dimensions;
    v3 colour = entity->colour;
    RendererDrawCube(position, dimensions, colour);
}

static void UpdateEntity(Entity* entity, f32 delta)
{
    if (entity->flags & ENTITY_PLAYER_BIT)
    {
        UpdatePlayer(entity);
    }
    else if (entity->flags & ENTITY_FREE_FLY_CAMERA_BIT)
    {
        UpdateFreeFlyingCamera(entity, delta);
    }
}

static void SetCamera(World* world)
{
    Entity* camera = world->current_camera;
    v3 position = PositionFromTransform(EntityGetWorldTransform(camera));
    v3 direction = Q4Rotate(camera->rotation, NEG_Z_AXIS);
    RendererSetCamera(position, direction);
}

void WorldUpdate(Arena* arena, World* world, f32 delta, b16 edit_mode)
{
    for (u16 i = 0; i < world->entities_num; i++)
    {
        Entity* entity = &world->entities[i];
        if (!edit_mode)
            UpdateEntity(entity, delta);
        if (entity->flags & ENTITY_VISIBLE_BIT)
            RenderEntity(entity);
    }
    PhysicsUpdate(arena, world->entities, world->entities_num, delta);
    SetCamera(world);
}

u32 WorldSpawnEntity(World* world, Entity entity)
{
    for (u32 i = 0; i < MAX_ENTITIES; i++)
    {
        if (world->entities[i].flags == ENTITY_NONE)
        {
            world->entities[i] = entity;
            world->entities_num = max(world->entities_num, i + 1);
            return i;
        }
    }

    ERROR("Failed to spawn entity. MAX_ENTITIES is too small.");
    return -1;
}

void WorldKillEntity(World* world, u32 entity_id)
{
    // Sets flags to ENTITY_NONE
    cu_memset(&world->entities[entity_id], 0, sizeof(Entity));
}

void WorldEntityMove(World* world, u32 entity_id, v3 velocity)
{
    world->entities[entity_id].velocity = velocity;
}

void WorldEntitySetName(World* world, u32 entity_id, const char* name)
{
    world->entities[entity_id].name = name;
}

void WorldEntitySetPosition(World* world, u32 entity_id, v3 position)
{
    world->entities[entity_id].position = position;
}

void WorldEntitySetRotation(World* world, u32 entity_id, q4 rotation)
{
    world->entities[entity_id].rotation = rotation;
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

const char* WorldEntityGetName(World* world, u32 entity_id)
{
    return world->entities[entity_id].name;
}

v3 WorldEntityGetPosition(World* world, u32 entity_id)
{
    return world->entities[entity_id].position;
}

q4 WorldEntityGetRotation(World* world, u32 entity_id)
{
    return world->entities[entity_id].rotation;
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

void WorldEntityAddChild(Arena* arena, World* world, u32 parent, u32 child)
{
    EntityAddChild(arena, &world->entities[parent], &world->entities[child]);
}