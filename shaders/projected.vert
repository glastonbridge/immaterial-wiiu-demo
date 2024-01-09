#version 450
layout(location=0) in vec4 in_color;
layout(location=1) in vec3 in_position;
layout(location=2) in vec2 in_texcoord;
uniform Projection {
    mat4 projection;
};
layout(location=0) out vec4 out_color;
layout(location=1) out vec2 out_texcoord;
void main()
{
    gl_Position = projection * vec4(in_position,1.0);
    out_color = in_color;
    out_texcoord = in_texcoord;
}
