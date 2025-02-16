#include "entity.h"
#include "error.h"
#include "input.h"
#include "math/constants.h"
#include "math/qat.h"
#include "math/scalar.h"
#include "cimgui.h"
#include "culibc.h"


m4 EntityGetLocalTransform(Entity* entity)
{
    m4 T = M4Translate(entity->position);
    m4 R = Q4ToMatrix(entity->rotation);
    // m4 S = M4Scale(entity->dimensions); // Only used by renderer right now

    return M4Multiply(T, R);
}

m4 EntityGetWorldTransform(Entity* entity)
{
    if (!entity->parent)
        return EntityGetLocalTransform(entity);
    else
        return M4Multiply(EntityGetWorldTransform(entity->parent), EntityGetLocalTransform(entity));
}

const char* EntityGetName(Entity* entity)
{
    if (entity->name)
        return entity->name;
    else
        return "Unnamed entity";
}

void EntityAddChild(Arena* arena, Entity* entity, Entity* child)
{
    if (entity->children == NULL)
        entity->children = PushArray(arena, Entity, ENTITY_MAX_CHILDREN_NUM);

    if (entity->children_num == ENTITY_MAX_CHILDREN_NUM)
        ERROR("Entity %s has too many children", EntityGetName(entity));

    child->parent = entity;
    entity->children[entity->children_num++] = *child;
}

static v3 CalcForwardDirection(Entity* entity)
{
    v3 local_forward = NEG_Z_AXIS;
    return Q4Rotate(entity->rotation, local_forward);
}

static v3 CalcRightDirection(v3 forward)
{
    v3 up = Y_AXIS;
    v3 horizontal = V3Normalize(v3(forward.x, 0.0f, forward.z));
    v3 perpendicular = V3Cross(horizontal, up);
    return V3Normalize(perpendicular);
}

// TODO: Handle corner cases with min and max pitch
static void UpdateFreeFlyingCameraRotation(Entity* camera)
{
    static const f32 max_pitch =  0.5f * PI - 0.017f;
    static const f32 min_pitch = -0.5f * PI + 0.017f;
    static const f32 sensitivity = 1.0f;

    q4 rotation = camera->rotation;

    v2 delta = GetMouseDelta();
    f32 yaw_delta   = sensitivity * delta.x;
    f32 pitch_delta = sensitivity * delta.y;

    v3 forward = CalcForwardDirection(camera);
    v3 right = CalcRightDirection(forward);

    q4 yaw_rotation = Q4FromAngleAxis(-yaw_delta, Y_AXIS);
    q4 pitch_rotation = Q4FromAngleAxis(pitch_delta, right);

    // Apply the rotations in the correct order: yaw first, then pitch
    rotation = Q4Multiply(pitch_rotation, rotation);  // Rotate around local X
    rotation = Q4Multiply(yaw_rotation, rotation);    // Rotate around world Y

    camera->rotation = Q4Normalize(rotation);
}

static void UpdateFreeFlyingCameraPosition(Entity* camera, f32 delta)
{
    v3* position  = &camera->position;
    v3  direction = CalcForwardDirection(camera);
    v3  velocity  = V3MulScalar(camera->velocity, delta);
    v3  right     = CalcRightDirection(direction);
    v3  up        = Y_AXIS;

    if (GetKeyState(KEY_SHIFT) & KEY_STATE_DOWN)
        velocity = V3MulScalar(velocity, 3.0f);

    if (GetKeyState(KEY_W) & KEY_STATE_DOWN)
        *position = V3Add(*position, V3Mul(velocity, direction));
    else if (GetKeyState(KEY_S) & KEY_STATE_DOWN)
        *position = V3Subtract(*position, V3Mul(velocity, direction));
    if (GetKeyState(KEY_A) & KEY_STATE_DOWN)
        *position = V3Subtract(*position, V3Mul(velocity, right));
    else if (GetKeyState(KEY_D) & KEY_STATE_DOWN)
        *position = V3Add(*position, V3Mul(velocity, right));
    if (GetKeyState(KEY_Q) & KEY_STATE_DOWN)
        *position = V3Subtract(*position, V3Mul(velocity, up));
    else if (GetKeyState(KEY_E) & KEY_STATE_DOWN
          || GetKeyState(KEY_SPACE) & KEY_STATE_DOWN)
        *position = V3Add(*position, V3Mul(velocity, up));
}

void UpdateFreeFlyingCamera(Entity* camera, f32 delta)
{
    UpdateFreeFlyingCameraPosition(camera, delta);
    UpdateFreeFlyingCameraRotation(camera);
}

Entity CreateFreeFlyingCameraEntity(v3 pos)
{
    Entity e = {
        .name = "Free flying camera",
        .flags = ENTITY_FREE_FLY_CAMERA_BIT | ENTITY_STATIC_BIT,
        .position = pos,
        .rotation = Q4Identity(),
        .velocity = v3(0.0005f, 0.0005f, 0.0005f),
    };

    return e;
}

Entity CreateCameraEntity(const char* name, v3 pos, v3 vel, q4 rot)
{
    Entity e = {
        .name = name,
        .flags = ENTITY_CAMERA_BIT,
        .position = pos,
        .rotation = rot,
        .velocity = vel,
    };

    return e;
}

Entity CreateStaticEntity(const char* name, v3 pos, v3 dimensions, v3 colour)
{
    Entity e = {
        .name = name,
        .flags = ENTITY_STATIC_BIT | ENTITY_COLLIDES_BIT | ENTITY_VISIBLE_BIT,
        .position = pos,
        .dimensions = dimensions,
        .colour = colour
    };

    return e;
}

void UpdatePlayer(Entity* player)
{
    // Handle velocity updates
    v3 direction = CalcForwardDirection(player);
    v3 right = CalcRightDirection(direction);
    v3 velocity = v3(0.0f);

    if (GetKeyState(KEY_W) & KEY_STATE_DOWN)
        velocity = V3MulScalar(V3Add(velocity, direction), 0.01f);
    else if (GetKeyState(KEY_S) & KEY_STATE_DOWN)
        velocity = V3MulScalar(V3Subtract(velocity, direction), 0.01f);
    if (GetKeyState(KEY_Q) & KEY_STATE_DOWN)
        velocity = V3MulScalar(V3Subtract(velocity, right), 0.01f);
    else if (GetKeyState(KEY_E) & KEY_STATE_DOWN)
        velocity = V3MulScalar(V3Add(velocity, right), 0.01f);

    player->velocity = velocity;

    // Handle rotation
    static q4 rotations[4];

    // It's a so called "chałupnicza" method
    v3 up = Y_AXIS;
    rotations[0] = Q4FromAngleAxis(radians(  0), up); // - Z
    rotations[1] = Q4FromAngleAxis(radians( 90), up); // + X
    rotations[2] = Q4FromAngleAxis(radians(180), up); // + Z
    rotations[3] = Q4FromAngleAxis(radians(-90), up); // - X

    // Switch between pre-defined rotations
    static u8 rotation_idx = 0;
    if (GetKeyState(KEY_A) & KEY_STATE_PRESSED)
        rotation_idx = (rotation_idx + 1) % 4;
    else if (GetKeyState(KEY_D) & KEY_STATE_PRESSED)
        rotation_idx = (rotation_idx + 3) % 4;
    player->rotation = rotations[rotation_idx];

    // Update Player's camera rotation
    Entity* camera = &player->children[0];
    v3 world_camera_pos = PositionFromTransform(EntityGetWorldTransform(camera));
    v3 world_player_pos = PositionFromTransform(EntityGetWorldTransform(player));
    m4 look_at = LookAt(world_camera_pos, world_player_pos, up);
    camera->rotation = Q4FromMatrix(look_at);
}

Entity CreatePlayerEntity(v3 pos, v3 dimensions, v3 colour)
{
    Entity e = {
        .name = "Player",
        .flags = ENTITY_PLAYER_BIT | ENTITY_COLLIDES_BIT | ENTITY_GRAVITY_BIT | ENTITY_VISIBLE_BIT,
        .position = pos,
        .dimensions = dimensions,
        .colour = colour,
        .rotation = Q4Identity(),
    };

    return e;
}

Entity CreateEnemyEntity(const char* name, v3 pos, v3 dimensions, v3 colour)
{
    Entity e = {
        .name = name,
        .flags = ENTITY_ENEMY_BIT | ENTITY_COLLIDES_BIT | ENTITY_GRAVITY_BIT | ENTITY_VISIBLE_BIT,
        .position = pos,
        .dimensions = dimensions,
        .colour = colour
    };

    return e;
}