#include "scalar.h"
#include "log/log.h"

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