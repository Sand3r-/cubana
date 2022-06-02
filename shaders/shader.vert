#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
} ubo;

layout(push_constant) uniform PushView {
    mat4 view;
} pushView;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 theColor;

void main()
{
    gl_Position = ubo.proj * pushView.view * vec4(inPosition, 0.0f, 1.0f);
    theColor = inColour;
}
