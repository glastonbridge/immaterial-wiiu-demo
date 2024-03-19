#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;

layout(location=0) out vec2 out_texcoord;
layout(location=1) out vec4 extra_out;

layout(binding = 4) uniform ExtraParams {
    vec4 extra_params;
};

void main()
{
    gl_Position = vec4(in_position, 1.0);
    out_texcoord = in_texcoord;
    extra_out = extra_params;
}