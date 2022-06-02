#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
} ubo;

layout(push_constant) uniform PushView {
    mat4 view;
} pushView;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 theColor;

void main()
{
    vec2 offsets = 2.0f * vec2(gl_InstanceIndex % 8, gl_InstanceIndex / 8);
    vec3 newPos = vec3(inPosition.xy + offsets, inPosition.z);
    gl_Position = ubo.proj * pushView.view * vec4(newPos, 1.0f);
    theColor = inColour;
}
