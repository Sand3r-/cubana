glslc shaders/shader.vert -o build/bin/shaders/shader.vert.spv
glslc shaders/shader.frag -o build/bin/shaders/shader.frag.spv
glslc shaders/imgui_shader.vert -o build/bin/shaders/imgui_shader.vert.spv
glslc shaders/imgui_shader.frag -o build/bin/shaders/imgui_shader.frag.spv
glslc -fshader-stage=vert shaders/im3d.glsl -DPOINTS -DVERTEX_SHADER -o build/bin/shaders/im3d.points.vert.spv
glslc -fshader-stage=frag shaders/im3d.glsl -DPOINTS -DFRAGMENT_SHADER -o build/bin/shaders/im3d.points.frag.spv
glslc -fshader-stage=vert shaders/im3d.glsl -DLINES -DVERTEX_SHADER -o build/bin/shaders/im3d.lines.vert.spv
glslc -fshader-stage=geom shaders/im3d.glsl -DLINES -DGEOMETRY_SHADER -o build/bin/shaders/im3d.lines.geom.spv
glslc -fshader-stage=frag shaders/im3d.glsl -DLINES -DFRAGMENT_SHADER -o build/bin/shaders/im3d.lines.frag.spv
glslc -fshader-stage=vert shaders/im3d.glsl -DTRIANGLES -DVERTEX_SHADER -o build/bin/shaders/im3d.triangles.vert.spv
glslc -fshader-stage=frag shaders/im3d.glsl -DTRIANGLES -DFRAGMENT_SHADER -o build/bin/shaders/im3d.triangles.frag.spv
