#version 450

layout(location = 0) in vec2 in_texcoord;
layout(binding = 0) uniform sampler2D tex_sampler;
layout(location = 1) in vec4 compose_params;

out vec4 out_color;

void main()
{
    float vignette = 1.0 - length(in_texcoord - vec2(0.5));
    vignette = clamp(vignette, 0.0, 1.0);
    out_color = texture(tex_sampler, in_texcoord) * vignette;
}
