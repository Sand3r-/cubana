#include "scalar_impl.h"
#include "log/log.h"
#include "constants.h"
#include <math.h>

f32 copysignf32(f32 mag, f32 sign)
{
    u32 mag_bits = *((u32*)&mag);   // Get bit representation of exp + mantissa
    u32 sign_bits = *((u32*)&sign); // Get bit representation of sign

    mag_bits &= 0x7FFFFFFF; // Clear sign bit (ensuring it's positive)
    sign_bits &= 0x80000000; // Extract only the sign bit

    u32 result = mag_bits | sign_bits; // Combine and return as float

    return *((f32*)&(result)); // Combine and return as float
}

i32 absi32(i32 a)
{
    return a < 0 ? -a : a;
}

f32 absf32(f32 a)
{
    return a < 0.0f ? -a : a;
}

f32 sqrtf32(f32 a)
{
    return sqrtf(a);
}

u64 minu64(u64 a, u64 b)
{
    return a < b ? a : b;
}

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

u64 maxu64(u64 a, u64 b)
{
    return a > b ? a : b;
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

u64 clampu64(u64 value, u64 min, u64 max)
{
    return value < min ? min : ((value > max) ? max : value);
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

f32 sinf32(f32 a)
{
    return sinf(a);
}

f32 cosf32(f32 a)
{
    return cosf(a);
}

f32 tanf32(f32 a)
{
    return tanf(a);
}

f32 asinf32(f32 a)
{
    return asinf(a);
}

f32 atanf32(f32 a, f32 b)
{
    return atan2f(a, b);
}

f32 radians(f32 degrees)
{
    return degrees * PI / 180.0f;
}
