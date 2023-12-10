#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_colour;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_colour;
layout(location = 1) out vec2 out_uv;

void main() {
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(in_position, 0.0, 1.0);
    out_colour = in_colour;
    out_uv = in_uv;
}