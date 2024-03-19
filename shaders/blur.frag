#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec4 blur_params;
layout(binding = 0) uniform sampler2D tex_sampler;

out vec4 out_color;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
void main()
{
    float scale = blur_params.x;
    float horizontal = blur_params.y;
    vec2 tex_offset = scale / vec2(1280, 720) * vec2(horizontal, 1.0 - horizontal);
    vec4 center_tap = texture(tex_sampler, in_texcoord);
    float scale_pos = texture(tex_sampler, in_texcoord + tex_offset * center_tap.a).a;
    float scale_neg = texture(tex_sampler, in_texcoord - tex_offset * center_tap.a).a;
    vec4 result = center_tap * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; ++i) {
        result += texture(tex_sampler, in_texcoord + tex_offset * float(i) * scale_pos) * weight[i];
        result += texture(tex_sampler, in_texcoord - tex_offset * float(i) * scale_neg) * weight[i];
    }
    out_color = result;
}
