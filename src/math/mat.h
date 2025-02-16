#pragma once
#include "vec.h"

// Matrices are handled assuming Column-Major order due to Vulkan requirements,
// meaning you access elements by specifying
// m[column][row]
// https://en.wikipedia.org/wiki/Row-_and_column-major_order
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


m4 M4Init(f32 diagonal);
m4 M4Multiply(m4 a, m4 b);
v4 M4MultiplyV4(m4 a, v4 b);
m4 Perspective(f32 fov_deg, f32 aspect, f32 near_z, f32 far_z);
m4 LookAt(v3 eye, v3 target, v3 up);
m4 Inverse(m4 m);
f32 Determinant(m4 m);

m4 M4Translate(v3 translation);
m4 M4Rotate(v3 euler);
m4 M4Scale(v3 scale);

v3 PositionFromTransform(m4 m);
v3 ScaleFromTransform(m4 m);
v3 EulerFromTransform(m4 m);

// Debug
void PrintMatrix(m4 m);
