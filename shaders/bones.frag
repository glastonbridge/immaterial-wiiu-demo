#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_normal;

layout(binding = 0) uniform sampler2D tex_sampler;

out vec4 out_color;

void main()
{
    vec3 ld = normalize(vec3(0.0f,-10.0f, 0.0f));
    vec3 n = normalize(in_normal);
    out_color = texture(tex_sampler, in_texcoord) * vec4(vec3(0.5f + 0.5f * dot(ld, in_normal)), 1.0f);
}
