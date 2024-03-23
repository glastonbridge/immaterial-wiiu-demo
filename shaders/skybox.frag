#version 450

layout(location = 0) in vec3 in_texcoord;
layout(binding = 0) uniform samplerCube tex_sky_sampler;

out vec4 out_color;

void main()
{             
    vec3 norm_texcoord = normalize(in_texcoord);
    out_color = vec4(texture(tex_sky_sampler, norm_texcoord).rgb, 1.0);
}
