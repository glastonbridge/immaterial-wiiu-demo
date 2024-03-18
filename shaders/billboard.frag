#version 450

layout(location = 0) in vec2 in_texcoord;
layout(binding = 0) uniform sampler2D tex_sampler;

out vec4 out_color;

// single pass hexagonal bokeh dof blur
vec3 hexablur(sampler2D tex, vec2 uv) {
    vec2 scale = vec2(1.0) / vec2(1920.0, 1080.0);
    vec3 col = vec3(0.0);
    float asum = 0.0;
    float coc = texture(tex, uv).a;
    
    // early out if coc is very small
    if (coc <= 0.001) {
        return texture(tex, uv).rgb;
    }

    // the actual blur
    // change the 16 to a higher number for more quality or lower for less
    for(float t = 0.0; t < 8.0 * 2.0 * 3.14; t += 3.14 / 16.0) {
    	float r = cos(3.14 / 6.0) / cos(mod(t, 2.0 * 3.14 / 6.0) - 3.14 / 6.0);
        
        // Tap filter once for coc
        vec2 offset = vec2(sin(t), cos(t)) * r * t * scale * coc;
        vec4 samp = texture(tex, uv + offset * 1.0);
        
        // Tap filter with coc from texture
        offset = vec2(sin(t), cos(t)) * r * t * scale * samp.a;
        samp = texture(tex, uv + offset * 1.0);
        
        // weigh and save
        col += samp.rgb * samp.a * t;
        asum += samp.a * t;
        
    }
    col = col / asum;
    return(col);
}

void main()
{
    float vignette = 1.0 - length(in_texcoord - vec2(0.5));
    vignette = clamp(vignette, 0.0, 1.0);
    out_color = vec4(hexablur(tex_sampler, in_texcoord), 1.0) * vignette;
    //out_color = texture(tex_sampler, in_texcoord) * vignette;
    //out_color = vec4(texture(tex_sampler, in_texcoord).a);
}
