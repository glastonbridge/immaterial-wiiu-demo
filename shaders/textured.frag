#version 450
layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;
layout(binding = 0) uniform sampler2D tex_sampler;
out vec4 out_color;
void main()
{
    vec3 ld = normalize(vec3(2.,-4, 3));
    out_color = texture(tex_sampler, in_texcoord)*vec4(vec3(0.5+0.5*dot(ld,in_normal)),1.0);
}
