@echo off
pushd shaders
    glslc.exe shader.vert -o shader.vert.spv
    glslc.exe shader.frag -o shader.frag.spv
    glslc.exe -fshader-stage=vert im3d.glsl -DPOINTS -DVERTEX_SHADER -o im3d.points.vert.spv
    glslc.exe -fshader-stage=frag im3d.glsl -DPOINTS -DFRAGMENT_SHADER -o im3d.points.frag.spv
    glslc.exe -fshader-stage=vert im3d.glsl -DLINES -DVERTEX_SHADER -o im3d.lines.vert.spv
    glslc.exe -fshader-stage=geom im3d.glsl -DLINES -DGEOMETRY_SHADER -o im3d.lines.geom.spv
    glslc.exe -fshader-stage=frag im3d.glsl -DLINES -DFRAGMENT_SHADER -o im3d.lines.frag.spv
    glslc.exe -fshader-stage=vert im3d.glsl -DTRIANGLES -DVERTEX_SHADER -o im3d.triangles.vert.spv
    glslc.exe -fshader-stage=frag im3d.glsl -DTRIANGLES -DFRAGMENT_SHADER -o im3d.triangles.frag.spv
popd shaders