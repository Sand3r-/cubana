#include "scalar.h"
#include "log/log.h"
#include "constants.h"

u32 minu32(u32 a, u32 b)
{
    return a < b ? a : b;
}
i32 mini32(i32 a, i32 b)
{
    return a < b ? a : b;
}
f32 minf32(f32 a, f32 b)
{
    return a < b ? a : b;
}

u32 maxu32(u32 a, u32 b)
{
    return a > b ? a : b;
}

i32 maxi32(i32 a, i32 b)
{
    return a > b ? a : b;
}

f32 maxf32(f32 a, f32 b)
{
    return a > b ? a : b;
}

u32 clampu32(u32 value, u32 min, u32 max)
{
    return value < min ? min : ((value > max) ? max : value);
}

i32 clampi32(i32 value, i32 min, i32 max)
{
    return value < min ? min : ((value > max) ? max : value);
}

f32 clampf32(f32 value, f32 min, f32 max)
{
    return value < min ? min : ((value > max) ? max : value);
}

f32 radians(f32 degrees)
{
    return degrees * PI / 180.0f;
}
