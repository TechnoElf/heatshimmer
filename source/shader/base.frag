#version 450

layout(binding = 1) uniform sampler2D tex;

layout(location = 0) in vec3 in_colour;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_colour;

void main() {
    vec4 tex_colour = texture(tex, in_uv);
    out_colour = vec4((in_colour + tex_colour.rgb) / 2.0, tex_colour.a);
}