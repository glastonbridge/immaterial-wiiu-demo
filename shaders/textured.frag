#version 450
layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(binding = 0) uniform sampler2D tex_sampler;
out vec4 out_color;
void main()
{
    out_color = texture(tex_sampler, in_texcoord) * in_color;
}
