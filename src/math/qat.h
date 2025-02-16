#pragma once
#include "types.h"
#include "math/vec.h"
#include "math/mat.h"

#define q4(...) (q4){ __VA_ARGS__ }

typedef union q4 q4;
union q4
{
    // x, y, z are complex part coefficients for i, j, k
    // w is a real part
    struct { f32 x, y, z, w; };
    f32 elems[4];
};

q4 Q4Identity(void);
q4 Q4FromAngleAxis(f32 angle, v3 axis);
q4 Q4Conjugate(q4 q);
f32 Q4Norm(q4 q);
q4 Q4Normalize(q4 q);
v3 Q4Rotate(q4 q, v3 v);
q4 Q4FromEuler(v3 angles);
v3 Q4ToEuler(q4 q);
q4 Q4Multiply(q4 a, q4 b);
q4 Q4MultiplyVec(q4 a, v3 v);
m4 Q4ToMatrix(q4 q);
q4 Q4FromMatrix(m4 m);