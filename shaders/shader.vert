#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConst {
    mat4 view;
    vec4 pos;
    vec4 col;
} pushConst;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 theColor;

void main()
{
    gl_Position = ubo.proj * pushConst.view * vec4(inPosition + pushConst.pos.xyz, 1.0f);
    theColor = pushConst.col.rgb;
}
