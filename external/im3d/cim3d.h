#pragma once
#include "math/vec.h"
#include "math/mat.h"

#if defined _WIN32 || defined __CYGWIN__
    #ifdef CIM3D_NO_EXPORT
        #define API
    #else
        #define API __declspec(dllexport)
    #endif
#else
    #ifdef __GNUC__
        #define API  __attribute__((__visibility__("default")))
    #else
        #define API
    #endif
#endif

#if defined __cplusplus
    #define EXTERN extern "C"
    #define ALIGNAS(x) alignas(x)
#else
    #define EXTERN extern
    #define ALIGNAS(x) _Alignas(x)
#endif

#define CIM3D_API EXTERN API
#define IM3D_VERTEX_ALIGNMENT 4

typedef enum im3dColor
{
    im3dColor_Black   = 0x000000ff,
    im3dColor_White   = 0xffffffff,
    im3dColor_Gray    = 0x808080ff,
    im3dColor_Red     = 0xff0000ff,
    im3dColor_Green   = 0x00ff00ff,
    im3dColor_Blue    = 0x0000ffff,
    im3dColor_Magenta = 0xff00ffff,
    im3dColor_Yellow  = 0xffff00ff,
    im3dColor_Cyan    = 0x00ffffff,
    im3dColor_Pink    = 0xffc0cbff,
    im3dColor_Orange  = 0xffa500ff,
    im3dColor_Gold    = 0xffd700ff,
    im3dColor_Brown   = 0x8b4513ff,
    im3dColor_Purple  = 0x800080ff,
    im3dColor_Teal    = 0x008080ff,
    im3dColor_Navy    = 0x000080ff,
} im3dColor;

typedef struct im3dVertexData
{
	ALIGNAS(IM3D_VERTEX_ALIGNMENT) v4   m_positionSize; // xyz = position, w = size
	ALIGNAS(IM3D_VERTEX_ALIGNMENT) u32  m_color;        // rgba8 (MSB = r) // TODO: Not sure about this alignment
} im3dVertexData;

typedef enum im3dKey
{
	im3dKey_Mouse_Left,
	im3dKey_Key_L,
	im3dKey_Key_R,
	im3dKey_Key_S,
	im3dKey_Key_T,

	im3dKey_Key_Count,

// the following map keys -> 'action' states which may be more intuitive
	im3dKey_Action_Select           = im3dKey_Mouse_Left,
	im3dKey_Action_GizmoLocal       = im3dKey_Key_L,
	im3dKey_Action_GizmoRotation    = im3dKey_Key_R,
	im3dKey_Action_GizmoScale       = im3dKey_Key_S,
	im3dKey_Action_GizmoTranslation = im3dKey_Key_T,

	im3dKey_Action_Count
} im3dKey;

typedef enum im3dDrawPrimitiveType
{
 // order here determines the order in which unsorted primitives are drawn
	DrawPrimitive_Triangles,
	DrawPrimitive_Lines,
	DrawPrimitive_Points,

	DrawPrimitive_Count
} im3dDrawPrimitiveType;

typedef struct im3dDrawList
{
	u32                   m_layerId;
	im3dDrawPrimitiveType m_primType;
	const im3dVertexData* m_vertexData;
	u32                   m_vertexCount;
} im3dDrawList;

typedef enum im3dFrustumPlane
{
	im3dFrustumPlane_Near,
	im3dFrustumPlane_Far,
	im3dFrustumPlane_Top,
	im3dFrustumPlane_Right,
	im3dFrustumPlane_Bottom,
	im3dFrustumPlane_Left,

	im3dFrustumPlane_Count
} im3dFrustumPlane;

typedef void (im3dDrawPrimitivesCallback)(const im3dDrawList* _drawList);
typedef struct im3dAppData
{
	bool   m_keyDown[im3dKey_Key_Count];          // Key states.
	v4     m_cullFrustum[im3dFrustumPlane_Count]; // Frustum planes for culling (if culling enabled).
	v3     m_cursorRayOrigin;                     // World space cursor ray origin.
	v3     m_cursorRayDirection;                  // World space cursor ray direction.
	v3     m_worldUp;                             // World space 'up' vector.
	v3     m_viewOrigin;                          // World space render origin (camera position).
	v3     m_viewDirection;                       // World space view direction.
	v2     m_viewportSize;                        // Viewport size (pixels).
	float  m_projScaleY;                          // Scale factor used to convert from pixel size -> world scale; use tan(fov) for perspective projections, far plane height for ortho.
	bool   m_projOrtho;                           // If the projection matrix is orthographic.
	float  m_deltaTime;                           // Time since previous frame (seconds).
	float  m_snapTranslation;                     // Snap value for translation gizmos (world units). 0 = disabled.
	float  m_snapRotation;                        // Snap value for rotation gizmos (radians). 0 = disabled.
	float  m_snapScale;                           // Snap value for scale gizmos. 0 = disabled.
	bool   m_flipGizmoWhenBehind;                 // Flip gizmo axes when viewed from behind.
	void*  m_appData;                             // App-specific data.

	im3dDrawPrimitivesCallback* drawCallback;     // e.g. void Im3d_Draw(const DrawList& _drawList)
} im3dAppData;

CIM3D_API im3dAppData* im3dGetAppData(void);

CIM3D_API void im3dNewFrame(void);
CIM3D_API void im3dEndFrame(void);

CIM3D_API u32 im3dGetDrawListCount(void);
CIM3D_API const im3dDrawList* im3dGetDrawLists(void);

CIM3D_API void im3dSetColor(u32 color);
CIM3D_API void im3dSetSize(f32 size);

CIM3D_API void im3dPushMatrix(m4 matrix);
CIM3D_API void im3dPopMatrix(void);
CIM3D_API void im3dPushDrawState(void);
CIM3D_API void im3dPopDrawState(void);

CIM3D_API void im3dDrawXyzAxes(void);
CIM3D_API void im3dDrawPoint(v3 position, float size, u32 color);
CIM3D_API void im3dDrawLine(v3 a, v3 b, float size, u32 color);
CIM3D_API void im3dDrawQuadFromPoints(v3 a, v3 b, v3 c, v3 d);
CIM3D_API void im3dDrawQuad(v3 origin, v3 normal, v2 size);
CIM3D_API void im3dDrawQuadFilledFromPoints(v3 a, v3 b, v3 c, v3 d);
CIM3D_API void im3dDrawQuadFilled(v3 origin, v3 normal, v2 size);
CIM3D_API void im3dDrawCircle(v3 origin, v3 normal, float radius, int detail); // detail = -1
CIM3D_API void im3dDrawCircleFilled(v3 origin, v3 normal, float radius, int detail); // detail = -1
CIM3D_API void im3dDrawSphere(v3 origin, float radius, int detail); // detail = -1
CIM3D_API void im3dDrawSphereFilled(v3 origin, float radius, int detail); // detail = -1
CIM3D_API void im3dDrawAlignedBox(v3 min, v3 max);
CIM3D_API void im3dDrawAlignedBoxFilled(v3 min, v3 max);
CIM3D_API void im3dDrawCylinder(v3 start, v3 end, float radius, int detail); // detail = -1
CIM3D_API void im3dDrawCapsule(v3 start, v3 end, float radius, int detail); // detail = -1
CIM3D_API void im3dDrawPrism(v3 start, v3 end, float radius, int sides);
CIM3D_API void im3dDrawArrow(v3 start, v3 end, float headLength, float headThickness); // headLength = -1.0f, headThickness = -1.0f
CIM3D_API bool im3dGizmo(const char* id, m4* transform);
