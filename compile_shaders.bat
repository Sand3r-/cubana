@echo off
pushd shaders
    glslc.exe shader.vert -o shader.vert.spv
    glslc.exe shader.frag -o shader.frag.spv
popd shaders