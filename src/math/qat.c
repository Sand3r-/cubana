#include "qat.h"
#include "math/constants.h"
#include "math/scalar.h"

q4 Q4Identity()
{
    return q4(0.0f, 0.0f, 0.0f, 1.0f);
}

q4 Q4FromAngleAxis(f32 angle, v3 axis)
{
    // Divide angle by 2, otherwise we would rotate it by 2 * angle
    f32 half_angle = angle * 0.5f;
    f32 sin_half_angle = sin(half_angle);
    f32 cos_half_angle = cos(half_angle);

    f32 w = cos_half_angle;
    f32 x = axis.x * sin_half_angle;
    f32 y = axis.y * sin_half_angle;
    f32 z = axis.z * sin_half_angle;

    return q4(x, y, z, w);
}

q4 Q4Conjugate(q4 q)
{
    return q4(-q.x, -q.y, -q.z, q.w);
}

f32 Q4Norm(q4 q)
{
    return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

q4 Q4Normalize(q4 q)
{
    f32 norm = Q4Norm(q);
    return q4(q.x / norm, q.y / norm, q.z / norm, q.w / norm);
}

v3 Q4Rotate(q4 q, v3 v)
{
    // Based on https://www.3dgep.com/understanding-quaternions/#rotations
    // To obtain a pure quaternion and to maintain the norm of the vector,
    // we multiply by the conjugate of q.
    q4 q_star = Q4Conjugate(q);
    q4 q_result = Q4Multiply(Q4MultiplyVec(q, v), q_star);
    v3 v_result = v3(q_result.x, q_result.y, q_result.z);
    return v_result;
}

q4 Q4Multiply(q4 a, q4 b)
{
    return q4(
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
}

q4 Q4MultiplyVec(q4 a, v3 v)
{
    // Convert vec to a quaternion representation to use Q4Multiply
    q4 vec = q4(v.x, v.y, v.z, 0.0f);
    q4 q_result = Q4Multiply(a, vec);
    return q_result;
}

q4 Q4FromEuler(v3 angles)
{
    f32 half_x = angles.x * 0.5f; // Yaw
    f32 half_y = angles.y * 0.5f; // Pitch
    f32 half_z = angles.z * 0.5f; // Roll

    f32 cx = cos(half_x);
    f32 sx = sin(half_x);
    f32 cy = cos(half_y);
    f32 sy = sin(half_y);
    f32 cz = cos(half_z);
    f32 sz = sin(half_z);

    return q4(
        sx * cy * cz - cx * sy * sz, // x
        cx * sy * cz + sx * cy * sz, // y
        cx * cy * sz - sx * sy * cz, // z
        cx * cy * cz + sx * sy * sz  // w
    );
}

v3 Q4ToEuler(q4 q)
{
    v3 angles;

    // Roll (Z)
    f32 sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    f32 cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    angles.z = atan2(sinr_cosp, cosr_cosp);

    // Pitch (Y)
    f32 sinp = 2.0f * (q.w * q.y - q.z * q.x);
    if (abs(sinp) >= 1.0f)
        angles.y = copysign(PI / 2.0f, sinp); // Gimbal lock at 90° or -90°
    else
        angles.y = asin(sinp);

    // Yaw (X)
    f32 siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    f32 cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    angles.x = atan2(siny_cosp, cosy_cosp);

    return angles;
}

m4 Q4ToMatrix(q4 q)
{
    m4 m = {0};

    f32 xx = q.x * q.x;
    f32 yy = q.y * q.y;
    f32 zz = q.z * q.z;
    f32 xy = q.x * q.y;
    f32 xz = q.x * q.z;
    f32 yz = q.y * q.z;
    f32 wx = q.w * q.x;
    f32 wy = q.w * q.y;
    f32 wz = q.w * q.z;

    m.m[0][0] = 1.0f - 2.0f * (yy + zz);
    m.m[1][0] = 2.0f * (xy - wz);
    m.m[2][0] = 2.0f * (xz + wy);
    m.m[0][1] = 2.0f * (xy + wz);
    m.m[1][1] = 1.0f - 2.0f * (xx + zz);
    m.m[2][1] = 2.0f * (yz - wx);
    m.m[0][2] = 2.0f * (xz - wy);
    m.m[1][2] = 2.0f * (yz + wx);
    m.m[2][2] = 1.0f - 2.0f * (xx + yy);
    m.m[3][3] = 1.0f; // Identity row

    return m;
}

// TODO: ensure this is correct for column-major
q4 Q4FromMatrix(m4 m)
{
    q4 q;
    f32 trace = m.m[0][0] + m.m[1][1] + m.m[2][2]; // Sum of diagonal elements

    if (trace > 0.0f)
    {
        f32 s = sqrt(trace + 1.0f) * 2.0f; // 4 * qw
        q.w = 0.25f * s;
        q.x = (m.m[2][1] - m.m[1][2]) / s;
        q.y = (m.m[0][2] - m.m[2][0]) / s;
        q.z = (m.m[1][0] - m.m[0][1]) / s;
    }
    else
    {
        if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2])
        {
            f32 s = sqrt(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f; // 4 * qx
            q.w = (m.m[2][1] - m.m[1][2]) / s;
            q.x = 0.25f * s;
            q.y = (m.m[0][1] + m.m[1][0]) / s;
            q.z = (m.m[0][2] + m.m[2][0]) / s;
        }
        else if (m.m[1][1] > m.m[2][2])
        {
            f32 s = sqrt(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f; // 4 * qy
            q.w = (m.m[0][2] - m.m[2][0]) / s;
            q.x = (m.m[0][1] + m.m[1][0]) / s;
            q.y = 0.25f * s;
            q.z = (m.m[1][2] + m.m[2][1]) / s;
        }
        else
        {
            f32 s = sqrt(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f; // 4 * qz
            q.w = (m.m[1][0] - m.m[0][1]) / s;
            q.x = (m.m[0][2] + m.m[2][0]) / s;
            q.y = (m.m[1][2] + m.m[2][1]) / s;
            q.z = 0.25f * s;
        }
    }

    return q;
}
