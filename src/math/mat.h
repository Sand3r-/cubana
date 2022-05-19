#pragma once
#include "vec.h"

typedef union m4 m4;
union m4
{
    v4 rows[4];
    f32 elems[4][4];
};

typedef union m3 m3;
union m3
{
    v3 rows[3];
    f32 elems[3][3];
};

// Column-Major is assumed
m4 M4Init(f32 diagonal);
m4 M4Multiply(m4 a, m4 b);
m4 Perspective(f32 width, f32 height, f32 near_z, f32 far_z);
m4 LookAt(v3 eye, v3 target, v3 up);