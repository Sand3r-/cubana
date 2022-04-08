#pragma once
#include "types.h"

#define min(a, b) _Generic((a), u32: minu32, i32: mini32, f32: minf32)(a, b)
#define max(a, b) _Generic((a), u32: maxu32, i32: maxi32, f32: maxf32)(a, b)

u32 minu32(u32 a, u32 b);
i32 mini32(i32 a, i32 b);
f32 minf32(f32 a, f32 b);

u32 maxu32(u32 a, u32 b);
i32 maxi32(i32 a, i32 b);
f32 maxf32(f32 a, f32 b);