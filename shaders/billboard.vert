#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;

layout(location=0) out vec2 out_texcoord;

void main()
{
    gl_Position = vec4(in_position, 1.0);
    out_texcoord = in_texcoord;
}