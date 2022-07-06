#include "math/vec.h"
#include <math.h>

f32 V3Length(v3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

v3 V3Normalize(v3 v)
{
    f32 length = V3Length(v);
    return v3(v.x / length, v.y / length, v.z / length);
}

f32 V3Dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

v3 V3Cross(v3 a, v3 b)
{
    return v3(a.y * b.z - a.z * b.y,
              a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}

v3 V3Add(v3 a, v3 b)
{
    return v3(a.x + b.x, a.y + b.y, a.z + b.z);
}

v3 V3Subtract(v3 a, v3 b)
{
    return v3(a.x - b.x, a.y - b.y, a.z - b.z);
}

v3 V3Mul(v3 a, v3 b)
{
    return v3(a.x * b.x, a.y * b.y, a.z * b.z);
}

v3 V3MulScalar(v3 v, f32 s)
{
    return v3(v.x * s, v.y * s, v.z * s);
}
