@echo off
pushd shaders
    REM Compile
    glslc.exe shader.vert -o shader.vert.spv
    glslc.exe shader.frag -o shader.frag.spv
    glslc.exe imgui_shader.vert -o imgui_shader.vert.spv
    glslc.exe imgui_shader.frag -o imgui_shader.frag.spv
    glslc.exe -fshader-stage=vert im3d.glsl -DPOINTS -DVERTEX_SHADER -o im3d.points.vert.spv
    glslc.exe -fshader-stage=frag im3d.glsl -DPOINTS -DFRAGMENT_SHADER -o im3d.points.frag.spv
    glslc.exe -fshader-stage=vert im3d.glsl -DLINES -DVERTEX_SHADER -o im3d.lines.vert.spv
    glslc.exe -fshader-stage=geom im3d.glsl -DLINES -DGEOMETRY_SHADER -o im3d.lines.geom.spv
    glslc.exe -fshader-stage=frag im3d.glsl -DLINES -DFRAGMENT_SHADER -o im3d.lines.frag.spv
    glslc.exe -fshader-stage=vert im3d.glsl -DTRIANGLES -DVERTEX_SHADER -o im3d.triangles.vert.spv
    glslc.exe -fshader-stage=frag im3d.glsl -DTRIANGLES -DFRAGMENT_SHADER -o im3d.triangles.frag.spv

    REM Copy
    xcopy /y shader.vert.spv "..\build\bin\shaders"
    xcopy /y shader.frag.spv "..\build\bin\shaders"
    xcopy /y imgui_shader.vert.spv "..\build\bin\shaders"
    xcopy /y imgui_shader.frag.spv "..\build\bin\shaders"
    xcopy /y im3d.points.vert.spv "..\build\bin\shaders"
    xcopy /y im3d.points.frag.spv "..\build\bin\shaders"
    xcopy /y im3d.lines.vert.spv "..\build\bin\shaders"
    xcopy /y im3d.lines.geom.spv "..\build\bin\shaders"
    xcopy /y im3d.lines.frag.spv "..\build\bin\shaders"
    xcopy /y im3d.triangles.vert.spv "..\build\bin\shaders"
    xcopy /y im3d.triangles.frag.spv "..\build\bin\shaders"
popd shaders