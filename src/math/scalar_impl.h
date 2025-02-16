#pragma once
#include "types.h"

f32 copysignf32(f32 mag, f32 sign);

i32 absi32(i32 a);
f32 absf32(f32 a);

f32 sqrtf32(f32 a);

u64 minu64(u64 a, u64 b);
u32 minu32(u32 a, u32 b);
i32 mini32(i32 a, i32 b);
f32 minf32(f32 a, f32 b);

u64 maxu64(u64 a, u64 b);
u32 maxu32(u32 a, u32 b);
i32 maxi32(i32 a, i32 b);
f32 maxf32(f32 a, f32 b);

u64 clampu64(u64 value, u64 min, u64 max);
u32 clampu32(u32 value, u32 min, u32 max);
i32 clampi32(i32 value, i32 min, i32 max);
f32 clampf32(f32 value, f32 min, f32 max);

f32 sinf32(f32 a);
f32 cosf32(f32 a);
f32 tanf32(f32 a);
f32 asinf32(f32 a);
f32 atanf32(f32 a, f32 b);

f32 radians(f32 degrees);