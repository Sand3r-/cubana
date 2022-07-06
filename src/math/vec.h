#pragma once
#include "types.h"

typedef union v2 v2;
union v2 {

    struct {
        f32 x;
        f32 y;
    };

    struct {
        f32 width;
        f32 height;
    };

    float elements[2];
};

typedef union v3 v3;
union v3 {

    struct {
        f32 x;
        f32 y;
        f32 z;
    };

    struct {
        f32 r;
        f32 g;
        f32 b;
    };

    float elements[3];
};

typedef union v4 v4;
union v4 {

    struct {
        f32 x;
        f32 y;

        union {
            struct {
                f32 z;

                union {
                    f32 w;
                    f32 radius;
                };
            };
            struct {
                f32 width;
                f32 height;
            };
        };
    };

    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };

    float elements[4];
};

typedef union iv2 iv2;
union iv2
{
    struct
    {
        i32 x;
        i32 y;
    };

    struct
    {
        i32 width;
        i32 height;
    };

    i32 elements[2];
};

typedef union iv3 iv3;
union iv3
{
    struct
    {
        i32 x;
        i32 y;
        i32 z;
    };

    struct
    {
        i32 r;
        i32 g;
        i32 b;
    };

    i32 elements[3];
};

typedef union iv4 iv4;
union iv4
{
    struct
    {
        i32 x;
        i32 y;
        i32 z;
        i32 w;
    };

    struct
    {
        i32 r;
        i32 g;
        i32 b;
        i32 a;
    };

    i32 elements[4];
};

// C++ MSVC compiler doesn't support (type){<initializer_list>}, so C++ part
// only gets the simplest default initialisers. It doesn't matter much, because
// for C++ this header is only used for creating bindings with C++ libraries.
#ifndef __cplusplus
// Vector initialisers
inline v2 V2Default() { return (v2){0.0f, 0.0f}; }
inline v2 V2FromValue(f32 v) { return (v2){v, v}; }
inline v2 V2FromValues(f32 x, f32 y) { return (v2){x, y}; }

inline v3 V3Default() { return (v3){0.0f, 0.0f, 0.0f}; }
inline v3 V3FromValue(f32 v) { return (v3){v, v, v}; }
inline v3 V3FromV4(v4 v) { return (v3){v.x, v.y, v.z}; }
inline v3 V3FromV2(v2 v) { return (v3){v.x, v.y}; }
inline v3 V3FromV2AndZ(v2 v, f32 z) { return (v3){v.x, v.y, z}; }
inline v3 V3FromValues(f32 x, f32 y, f32 z) { return (v3){x, y, z}; }

inline v4 V4Default() { return (v4){0.0f, 0.0f, 0.0f}; }
inline v4 V4FromValue(f32 v) { return (v4){v, v, v, v}; }
inline v4 V4FromV3(v3 v) { return (v4){v.x, v.y, v.z, 0.0f}; }
inline v4 V4FromV3AndW(v3 v, f32 w) { return (v4){v.x, v.y, v.z, w}; }
inline v4 V4FromV2(v2 v) { return (v4){v.x, v.y, 0.0f, 0.0f}; }
inline v4 V4FromV2AndZW(v2 v, f32 z, f32 w) { return (v4){v.x, v.y, z, w}; }
inline v4 V4FromValues(f32 x, f32 y, f32 z, f32 w) { return (v4){x, y, z, w}; }

// Implements overloading for v2, v3, v4 constructors.
// These functions pick the appropriate function/macro (expression in fact)
// passed based on the number of arguments given.
#define ARG_NUM_OVERLOAD_2(_0, _1, _2, NAME, ...) NAME
#define ARG_NUM_OVERLOAD_3(_0, _1, _2, _3, NAME, ...) NAME
#define ARG_NUM_OVERLOAD_4(_0, _1, _2, _3, _4, NAME, ...) NAME

// If a given function needs to be overloaded based on parameter type, a helper
// macro utilising _Generic is used to provide the right name based on the type
// Examples: V31Param or V41Param.
#define v2(...) ARG_NUM_OVERLOAD_2(_0, ##__VA_ARGS__, \
    V2FromValues, V2FromValue, V2Default)(__VA_ARGS__)

#define v3(...) ARG_NUM_OVERLOAD_3(_0, ##__VA_ARGS__, \
    V3FromValues, V3FromV2AndZ, V31Param, V3Default)(__VA_ARGS__)
#define V31Param(X) _Generic((X), v4: V3FromV4, v2: V3FromV2, f32: V3FromValue)(X)

#define v4(...) ARG_NUM_OVERLOAD_4(_0, ##__VA_ARGS__, \
    V4FromValues, V4FromV2AndZW, V4FromV3AndW, V41Param, V4Default)(__VA_ARGS__)
#define V41Param(X) _Generic((X), v3: V4FromV3, v2: V4FromV2, f32: V4FromValue)(X)

#else // __cplusplus

#define v2(...) (v2){ __VA_ARGS__ }
#define v3(...) (v3){ __VA_ARGS__ }
#define v4(...) (v4){ __VA_ARGS__ }

#endif // __cplusplus

#define iv2(...) (iv2){ __VA_ARGS__ }
#define iv3(...) (iv3){ __VA_ARGS__ }
#define iv4(...) (iv4){ __VA_ARGS__ }

f32 V3Length(v3 v);
v3 V3Normalize(v3 v);
f32 V3Dot(v3 a, v3 b);
v3 V3Cross(v3 a, v3 b);
v3 V3Add(v3 a, v3 b);
v3 V3Subtract(v3 a, v3 b);
v3 V3Mul(v3 a, v3 b);
v3 V3MulScalar(v3 v, f32 s);
