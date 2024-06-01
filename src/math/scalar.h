#pragma once
#include "types.h"

#define min(a, b) _Generic((a), u64: minu64, u32: minu32, i32: mini32, f32: minf32)(a, b)
#define max(a, b) _Generic((a), u64: maxu64, u32: maxu32, i32: maxi32, f32: maxf32)(a, b)
#define clamp(v, min, max) _Generic((v), u64: clampu64, u32: clampu32, i32: clampi32, f32: clampf32)(v, min, max)

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

f32 radians(f32 degrees);