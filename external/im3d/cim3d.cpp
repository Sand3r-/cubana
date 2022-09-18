#include "cim3d.h"
#include "im3d.h"

#define TO_MAT4(x) (const Im3d::Mat4&)x
#define TO_VEC3(x) (const Im3d::Vec3&)x
#define TO_VEC2(x) (const Im3d::Vec2&)x

CIM3D_API im3dAppData* im3dGetAppData(void)
{
    return (im3dAppData*)&Im3d::GetAppData();
}

CIM3D_API void im3dNewFrame(void)
{
    Im3d::NewFrame();
}

CIM3D_API void im3dEndFrame(void)
{
    Im3d::EndFrame();
}

CIM3D_API u32 im3dGetDrawListCount(void)
{
    return Im3d::GetDrawListCount();
}

CIM3D_API const im3dDrawList* im3dGetDrawLists(void)
{
    return (const im3dDrawList*)Im3d::GetDrawLists();
}

CIM3D_API void im3dSetColor(u32 color)
{
    Im3d::SetColor(color);
}

CIM3D_API void im3dSetSize(f32 size)
{
    Im3d::SetSize(size);
}

CIM3D_API void im3dPushMatrix(m4 matrix)
{
    Im3d::PushMatrix(TO_MAT4(matrix));
}

CIM3D_API void im3dPopMatrix()
{
    Im3d::PopMatrix();
}

CIM3D_API void im3dPushDrawState()
{
    Im3d::PushDrawState();
}

CIM3D_API void im3dPopDrawState()
{
    Im3d::PopDrawState();
}

CIM3D_API void im3dDrawXyzAxes(void)
{
    Im3d::DrawXyzAxes();
}

CIM3D_API void im3dDrawPoint(v3 position, float size, u32 color)
{
    Im3d::DrawPoint(TO_VEC3(position), size, color);
}

CIM3D_API void im3dDrawLine(v3 a, v3 b, float size, u32 color)
{
    Im3d::DrawLine(TO_VEC3(a), TO_VEC3(b), size, color);
}

CIM3D_API void im3dDrawQuadFromPoints(v3 a, v3 b, v3 c, v3 d)
{
    Im3d::DrawQuad(TO_VEC3(a), TO_VEC3(b), TO_VEC3(c), TO_VEC3(d));
}

CIM3D_API void im3dDrawQuad(v3 origin, v3 normal, v2 size)
{
    Im3d::DrawQuad(TO_VEC3(origin), TO_VEC3(normal), TO_VEC2(size));
}

CIM3D_API void im3dDrawQuadFilledFromPoints(v3 a, v3 b, v3 c, v3 d)
{
    Im3d::DrawQuadFilled(TO_VEC3(a), TO_VEC3(b), TO_VEC3(c), TO_VEC3(d));
}

CIM3D_API void im3dDrawQuadFilled(v3 origin, v3 normal, v2 size)
{
    Im3d::DrawQuadFilled(TO_VEC3(origin), TO_VEC3(normal), TO_VEC2(size));
}

CIM3D_API void im3dDrawCircle(v3 origin, v3 normal, float radius, int detail) // detail = -
{
    Im3d::DrawCircle(TO_VEC3(origin), TO_VEC3(normal), radius, detail);
}

CIM3D_API void im3dDrawCircleFilled(v3 origin, v3 normal, float radius, int detail) // detail = -
{
    Im3d::DrawCircleFilled(TO_VEC3(origin), TO_VEC3(normal), radius, detail);
}

CIM3D_API void im3dDrawSphere(v3 origin, float radius, int detail) // detail = -
{
    Im3d::DrawSphere(TO_VEC3(origin), radius, detail);
}

CIM3D_API void im3dDrawSphereFilled(v3 origin, float radius, int detail) // detail = -
{
    Im3d::DrawSphereFilled(TO_VEC3(origin), radius, detail);
}

CIM3D_API void im3dDrawAlignedBox(v3 min, v3 max)
{
    Im3d::DrawAlignedBox(TO_VEC3(min), TO_VEC3(max));
}

CIM3D_API void im3dDrawAlignedBoxFilled(v3 min, v3 max)
{
    Im3d::DrawAlignedBoxFilled(TO_VEC3(min), TO_VEC3(max));
}

CIM3D_API void im3dDrawCylinder(v3 start, v3 end, float radius, int detail) // detail = -
{
    Im3d::DrawCylinder(TO_VEC3(start), TO_VEC3(end), radius, detail);
}

CIM3D_API void im3dDrawCapsule(v3 start, v3 end, float radius, int detail) // detail = -
{
    Im3d::DrawCapsule(TO_VEC3(start), TO_VEC3(end), radius, detail);
}

CIM3D_API void im3dDrawPrism(v3 start, v3 end, float radius, int sides)
{
    Im3d::DrawPrism(TO_VEC3(start), TO_VEC3(end), radius, sides);
}

CIM3D_API void im3dDrawArrow(v3 start, v3 end, float headLength, float headThickness) // headLength = -1.0f, headThickness = -1.0
{
    Im3d::DrawArrow(TO_VEC3(start), TO_VEC3(end), headLength, headThickness);
}

CIM3D_API bool im3dGizmo(const char* id, m4* transform)
{
    return Im3d::Gizmo(id, transform->raw);
}
