im3d = {}
-- im3d.PushDrawState()
im3d.PushDrawState = im3dPushDrawState
-- im3d.PushMatrix(mat4 matrix)
im3d.PushMatrix = im3dPushMatrix
-- im3d.PopMatrix()
im3d.PopMatrix = im3dPopMatrix
-- im3d.PopDrawState()
im3d.PopDrawState = im3dPopDrawState
-- im3d.DrawXyzAxes()
im3d.DrawXyzAxes = im3dDrawXyzAxes
-- im3d.SetSize(f32 size)
im3d.SetSize = im3dSetSize
-- im3d.SetColor(im3d.Color color)
im3d.SetColor = im3dSetColor
-- im3d.DrawPoint(v3 position, f32 size, im3d.Color color)
im3d.DrawPoint = im3dDrawPoint
-- im3d.DrawLine(v3 from, v3 to, f32 size, im3d.Color color)
im3d.DrawLine = im3dDrawLine
-- im3d.DrawQuad(v3 origin, v3 normal, v2 size)
im3d.DrawQuad = im3dDrawQuad
-- im3d.DrawQuadFromPoints(v3 a, v3 b, v3 c, v3 d)
im3d.DrawQuadFromPoints = im3dDrawQuadFromPoints
-- im3d.DrawQuadFilled(v3 origin, v3 normal, v2 size)
im3d.DrawQuadFilled = im3dDrawQuadFilled
-- im3d.DrawQuadFilledFromPoints(v3 a, v3 b, v3 c, v3 d)
im3d.DrawQuadFilledFromPoints = im3dDrawQuadFilledFromPoints
-- im3d.DrawCircle(v3 origin, v3 normal, f32 radius, int detail = -1)
im3d.DrawCircle = function(origin, normal, radius, detail)
    detail = detail or (-1) -- default parameter
    im3dDrawCircle(origin, normal, radius, detail)
end
-- im3d.DrawCircleFilled(v3 origin, v3 normal, f32 radius, int detail = -1)
im3d.DrawCircleFilled = function(origin, normal, radius, detail)
    detail = detail or (-1) -- default parameter
    im3dDrawCircleFilled(origin, normal, radius, detail)
end
-- im3d.DrawSphere(v3 origin, float radius, int detail = -1)
im3d.DrawSphere = function(origin, radius, detail)
    detail = detail or (-1) -- default parameter
    im3dDrawSphere(origin, radius, detail)
end
-- im3d.DrawSphereFilled(v3 origin, float radius, int detail = -1)
im3d.DrawSphereFilled = function(origin, radius, detail)
    detail = detail or (-1) -- default parameter
    im3dDrawSphereFilled(origin, radius, detail)
end
-- im3d.DrawAlignedBox(v3 min, v3 max)
im3d.DrawAlignedBox = im3dDrawAlignedBox
-- im3d.DrawAlignedBoxFilled(v3 min, v3 max)
im3d.DrawAlignedBoxFilled = im3dDrawAlignedBoxFilled
-- im3d.DrawCylinder(v3 start, v3 end, float radius, int detail = -1)
im3d.DrawCylinder = function(start, ending, radius, detail)
    detail = detail or (-1) -- default parameter
    im3dDrawCylinder(start, ending, radius, detail)
end
-- im3d.DrawCapsule(v3 start, v3 end, float radius, int detail = -1)
im3d.DrawCapsule = function(start, ending, radius, detail)
    detail = detail or (-1) -- default parameter
    im3dDrawCapsule(start, ending, radius, detail)
end
-- im3d.DrawPrism(v3 start, v3 end, float radius, int sides)
im3d.DrawPrism = im3dDrawPrism
-- im3d.DrawArrow(v3 start, v3 end, float headLength = -1, float headThickness = -1)
im3d.DrawArrow = function(start, ending, head_length, head_thickness)
    head_length = head_length or (-1.0) -- default parameter
    head_thickness = head_thickness or (-1.0) -- default parameter
    im3dDrawArrow(start, ending, head_length, head_thickness)
end
-- was_changed, out_matrix = im3d.Gizmo(label, in_matrix)
im3d.Gizmo = im3dGizmo

im3d.Color_Black   = 0x000000ff
im3d.Color_White   = 0xffffffff
im3d.Color_Gray    = 0x808080ff
im3d.Color_Red     = 0xff0000ff
im3d.Color_Green   = 0x00ff00ff
im3d.Color_Blue    = 0x0000ffff
im3d.Color_Magenta = 0xff00ffff
im3d.Color_Yellow  = 0xffff00ff
im3d.Color_Cyan    = 0x00ffffff
im3d.Color_Pink    = 0xffc0cbff
im3d.Color_Orange  = 0xffa500ff
im3d.Color_Gold    = 0xffd700ff
im3d.Color_Brown   = 0x8b4513ff
im3d.Color_Purple  = 0x800080ff
im3d.Color_Teal    = 0x008080ff
im3d.Color_Navy    = 0x000080ff