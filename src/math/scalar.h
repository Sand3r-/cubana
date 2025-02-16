#pragma once
#include "scalar_impl.h"

#define copysign(a, b) _Generic((a), f32: copysignf32)(a, b)

#define abs(a) _Generic((a), i32: absi32, f32: absf32)(a)
#define sqrt(a) _Generic((a), f32: sqrtf32)(a)

#define min(a, b) _Generic((a), u64: minu64, u32: minu32, i32: mini32, f32: minf32)(a, b)
#define max(a, b) _Generic((a), u64: maxu64, u32: maxu32, i32: maxi32, f32: maxf32)(a, b)

#define clamp(v, min, max) _Generic((v), u64: clampu64, u32: clampu32, i32: clampi32, f32: clampf32)(v, min, max)

#define sin(a) _Generic((a), f32: sinf32)(a)
#define cos(a) _Generic((a), f32: cosf32)(a)
#define tan(a) _Generic((a), f32: tanf32)(a)
#define asin(a) _Generic((a), f32: asinf32)(a)
#define atan2(a, b) _Generic((a), f32: atanf32)(a, b)
