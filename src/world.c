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
    world->entities[world->entities_num++] = CreateFreeFlyingCameraEntity(position);
    return &world->entities[world->entities_num - 1];
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
            world->entities[world->entities_num++] =
                CreateStaticEntity(name, obj->position, obj->dimensions, obj->colour);
        }
    }

    // Add example player and enemy
    Entity player = CreatePlayerEntity(v3(0.0f, 10.0f, 0.0f), v3(1.0f, 1.0f, 1.0f), v3(0.64f, 1.f, 1.f));
    v3 camera_pos = v3(0.0f, 1.0f, 2.0f);
    v3 camera_vel = v3(0.5f);
    q4 camera_rot = Q4FromAngleAxis(radians(-30.0f), X_AXIS);
    Entity camera = CreateCameraEntity("Player's Camera", camera_pos, camera_vel, camera_rot);
    world->entities[world->entities_num++] = player;
    EntityAddChild(arena, &world->entities[world->entities_num - 1], &camera);
    world->current_camera = &world->entities[world->entities_num - 1].children[0];

    Entity enemy = CreateEnemyEntity("Enemy", v3(3.0f, 10.0f, 0.0f), v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f));
    world->entities[world->entities_num++] = enemy;
}

void WorldInit(Arena* arena, World* world)
{
    WorldLoadLevel(arena, world, "assets/TestLevel.cmt");
    Entity* camera = CreateFreeFlyingCamera(world);
    world->current_camera = camera;
}

static void RenderEntity(Entity* entity)
{
    v3 position = PositionFromTransform(EntityGetWorldTransform(entity));
    v3 dimensions = entity->dimensions;
    v3 colour = entity->colour;
    RendererDrawCube(position, dimensions, colour);
}

// TODO: Remove mouse_snap from here
static void UpdateEntity(Entity* entity, f32 delta, b16 mouse_snap)
{
    for (u32 c = 0; c < entity->children_num; c++)
    {
        UpdateEntity(&entity->children[c], delta, mouse_snap);
    }

    if (entity->flags & ENTITY_PLAYER_BIT)
    {
        UpdatePlayer(entity);
    }
    else if (entity->flags & ENTITY_FREE_FLY_CAMERA_BIT)
    {
        if (mouse_snap)
        {
            UpdateFreeFlyingCamera(entity, delta);
        }
    }

    if (entity->flags & ENTITY_VISIBLE_BIT)
    {
        RenderEntity(entity);
    }
}

static void SetCamera(World* world)
{
    Entity* camera = world->current_camera;
    v3 position = PositionFromTransform(EntityGetWorldTransform(camera));
    v3 direction = Q4Rotate(camera->rotation, NEG_Z_AXIS);
    RendererSetCamera(position, direction);
}

void WorldUpdate(Arena* arena, World* world, f32 delta, b16 mouse_snap)
{
    for (u16 i = 0; i < world->entities_num; i++)
    {
        UpdateEntity(&world->entities[i], delta, mouse_snap);
    }
    PhysicsUpdate(arena, world->entities, world->entities_num, delta);
    SetCamera(world);
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

