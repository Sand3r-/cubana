#include "math/mat.h"
#include "math/scalar.h"
#include <math.h>

m4 M4Init(f32 diagonal)
{
    m4 m = {
        .rows = {
            { diagonal                },
            { 0.f, diagonal           },
            { 0.f, 0.f, diagonal      },
            { 0.f, 0.f, 0.f, diagonal },
        }
    };
    return m;
}

m4 M4Multiply(m4 a, m4 b)
{
    m4 c;

    for(int j = 0; j < 4; ++j)
    {
        for(int i = 0; i < 4; ++i)
        {
            c.elems[i][j] = (a.elems[0][j]*b.elems[i][0] +
                             a.elems[1][j]*b.elems[i][1] +
                             a.elems[2][j]*b.elems[i][2] +
                             a.elems[3][j]*b.elems[i][3]);
        }
    }

    return c;
}

v4 M4MultiplyV4(m4 m, v4 v)
{
    return v4(m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
              m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
              m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
              m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w);
}

// Implementation based on https://vincent-p.github.io/posts/vulkan_perspective_matrix/
// Transforms eye space into a right-handed coordinate system
// Assumes Reverse-Depth (near = 1.0f, far = 0.0f)
// Remember to set compare op to Greater/equal and depth clearing to 0.0f appropriately
// Consult https://github.com/sebbbi/rust_test/commit/d64119ce22a6a4972e97b8566e3bbd221123fcbb
m4 Perspective(f32 fov_deg, f32 aspect, f32 near_z, f32 far_z)
{
    f32 fov_rad = radians(fov_deg);
    f32 focal_length = 1.0f / (tanf(fov_rad / 2.0f));
    m4 r = {
        .elems[0][0] = focal_length / aspect,
        .elems[1][1] = -focal_length,
        .elems[2][2] = near_z / (far_z - near_z),
        .elems[2][3] = -1,
        .elems[3][2] = near_z * far_z / (far_z - near_z)
    };
    return r;
}

m4 LookAt(v3 eye, v3 target, v3 up)
{
    v3 f = V3Normalize(V3Subtract(target, eye));
    v3 s = V3Normalize(V3Cross(f, up));
    v3 u = V3Cross(s, f);

    m4 result = {
        .elems[0][0] = s.x,
        .elems[0][1] = u.x,
        .elems[0][2] = -f.x,
        .elems[0][3] = 0.0f,

        .elems[1][0] = s.y,
        .elems[1][1] = u.y,
        .elems[1][2] = -f.y,
        .elems[1][3] = 0.0f,

        .elems[2][0] = s.z,
        .elems[2][1] = u.z,
        .elems[2][2] = -f.z,
        .elems[2][3] = 0.0f,

        .elems[3][0] = -V3Dot(s, eye),
        .elems[3][1] = -V3Dot(u, eye),
        .elems[3][2] = V3Dot(f, eye),
        .elems[3][3] = 1.0f,
    };

    return result;
}

m4 Inverse(m4 m)
{
    m4 ret = {
        .m[0][0] = m.m[1][2] * m.m[2][3] * m.m[3][1] - m.m[1][3] * m.m[2][2] * m.m[3][1] +
                   m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
                   m.m[1][2] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][2] * m.m[3][3],
        .m[0][1] = m.m[0][3] * m.m[2][2] * m.m[3][1] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
                   m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
                   m.m[0][2] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][2] * m.m[3][3],
        .m[0][2] = m.m[0][2] * m.m[1][3] * m.m[3][1] - m.m[0][3] * m.m[1][2] * m.m[3][1] +
                   m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
                   m.m[0][2] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][2] * m.m[3][3],
        .m[0][3] = m.m[0][3] * m.m[1][2] * m.m[2][1] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
                   m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
                   m.m[0][2] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][2] * m.m[2][3],
        .m[1][0] = m.m[1][3] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
                   m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
                   m.m[1][2] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][2] * m.m[3][3],
        .m[1][1] = m.m[0][2] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][2] * m.m[3][0] +
                   m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
                   m.m[0][2] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][2] * m.m[3][3],
        .m[1][2] = m.m[0][3] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
                   m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
                   m.m[0][2] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][2] * m.m[3][3],
        .m[1][3] = m.m[0][2] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][2] * m.m[2][0] +
                   m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
                   m.m[0][2] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][2] * m.m[2][3],
        .m[2][0] = m.m[1][1] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][1] * m.m[3][0] +
                   m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
                   m.m[1][1] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][1] * m.m[3][3],
        .m[2][1] = m.m[0][3] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
                   m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
                   m.m[0][1] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][1] * m.m[3][3],
        .m[2][2] = m.m[0][1] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[3][0] +
                   m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
                   m.m[0][1] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][1] * m.m[3][3],
        .m[2][3] = m.m[0][3] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
                   m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
                   m.m[0][1] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][1] * m.m[2][3],
        .m[3][0] = m.m[1][2] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
                   m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
                   m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[1][0] * m.m[2][1] * m.m[3][2],
        .m[3][1] = m.m[0][1] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][1] * m.m[3][0] +
                   m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
                   m.m[0][1] * m.m[2][0] * m.m[3][2] + m.m[0][0] * m.m[2][1] * m.m[3][2],
        .m[3][2] = m.m[0][2] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
                   m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
                   m.m[0][1] * m.m[1][0] * m.m[3][2] - m.m[0][0] * m.m[1][1] * m.m[3][2],
        .m[3][3] = m.m[0][1] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][1] * m.m[2][0] +
                   m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
                   m.m[0][1] * m.m[1][0] * m.m[2][2] + m.m[0][0] * m.m[1][1] * m.m[2][2],
    };
    float det = 1.0f / Determinant(m);
    for (int i = 0; i < 16; ++i)
    {
        ret.m[i / 4][i % 4] *= det;
    }

    return ret;
}

f32 Determinant(m4 m)
{
    return m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] -
           m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
           m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +
           m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0] +
           m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] -
           m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
           m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] +
           m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
           m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] -
           m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
           m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] +
           m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
           m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
           m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -
           m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +
           m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] +
           m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] -
           m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
           m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] +
           m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
           m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] -
           m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
           m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] +
           m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3];
}

