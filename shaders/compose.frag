#version 450

layout(location = 0) in vec2 in_texcoord;
layout(binding = 0) uniform sampler2D tex_sampler_blur;
layout(binding = 0) uniform sampler2D tex_sampler_orig;
layout(location = 1) in vec4 compose_params;

out vec4 out_color;

void main()
{   
    float vignette_darkness = compose_params.x;
    float fade = compose_params.y;
    float vignette = max(0.0, vignette_darkness - length(in_texcoord - vec2(0.5)));
    vignette = clamp(vignette, 0.0, 1.0);
    vec4 blur = texture(tex_sampler_blur, in_texcoord);
    vec4 orig = texture(tex_sampler_orig, in_texcoord);
    out_color = mix(orig, blur, smoothstep(0.0, 1.0, blur.a)) * vignette * fade;
}
