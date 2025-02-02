#include "entity.h"
#include "math/constants.h"
#include "math/scalar.h"
#include "input.h"
#include <math.h>

static v3 CalcCameraRightDirection(Entity* camera)
{
    v3 up = v3(0.0, 1.0f, 0.0f);
    v3 dir = camera->direction;
    v3 horizontal = V3Normalize(v3(dir.x, 0.0f, dir.z));
    v3 perpendicular = V3Cross(horizontal, up);
    return V3Normalize(perpendicular);
}

static void UpdateFreeFlyingCameraRotation(Entity* camera)
{
    static const f32 max_pitch =  0.5f * PI - 0.017f;
    static const f32 min_pitch = -0.5f * PI + 0.017f;
    f32 yaw = camera->rotation.yaw;
    f32 pitch = camera->rotation.pitch;

    v2 delta = GetMouseDelta();

    static const f32 sensitivity = 1.0f;
    yaw   += sensitivity * delta.x;
    pitch += sensitivity * delta.y;

    pitch = max(min(pitch, max_pitch), min_pitch);

    v3 forward;
    forward.x = cosf(pitch) * cosf(yaw);
    forward.y = sinf(pitch);
    forward.z = cosf(pitch) * sinf(yaw);

    camera->direction = V3Normalize(forward);
    camera->rotation.yaw = yaw;
    camera->rotation.pitch = pitch;
}

static void UpdateFreeFlyingCameraPosition(Entity* camera, f32 delta)
{
    v3* position  = &camera->position;
    v3  direction = camera->direction;
    v3  velocity  = V3MulScalar(camera->velocity, delta);
    v3  right     = CalcCameraRightDirection(camera);
    v3  up        = v3(0.0f, 1.0f, 0.0f);

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
        .type = ENTITY_FREE_FLY_CAMERA_BIT,
        .position = pos,
        .direction = v3(0.0f, 0.0f, -1.0f),
        .velocity = v3(0.0005f, 0.0005f, 0.0005f),
        .rotation = {
            .yaw = (3.0f / 2.0f) * PI,
            .pitch = 0.0f
        }
    };

    return e;
}

Entity CreateStaticEntity(v3 pos, v3 dimensions, v3 colour)
{
    Entity e = {
        .type = ENTITY_STATIC_BIT,
        .position = pos,
        .dimensions = dimensions,
        .colour = colour
    };

    return e;
}