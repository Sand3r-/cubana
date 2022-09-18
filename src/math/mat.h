#pragma once
#include "vec.h"

typedef union m4 m4;
union m4
{
    v4 rows[4];
    f32 elems[4][4];
    f32 m[4][4];
    f32 raw[16];
};

typedef union m3 m3;
union m3
{
    v3 rows[3];
    f32 elems[3][3];
    f32 m[3][3];
};

// Column-Major is assumed
m4 M4Init(f32 diagonal);
m4 M4Multiply(m4 a, m4 b);
v4 M4MultiplyV4(m4 a, v4 b);
m4 Perspective(f32 fov_deg, f32 aspect, f32 near_z, f32 far_z);
m4 LookAt(v3 eye, v3 target, v3 up);
m4 Inverse(m4 m);
f32 Determinant(m4 m);
