#version 450
layout(location=0) in vec4 in_color;
layout(location=1) in vec3 in_position;
layout(location=2) in vec2 in_texcoord;
layout(location=3) in vec3 in_normal;
layout(binding=0) uniform Projection {
    mat4 projection;
};
layout(binding=1) uniform Transform {
    mat4 transform;
};
layout(location=0) out vec4 out_color;
layout(location=1) out vec2 out_texcoord;
layout(location=2) out vec3 out_normal;
void main()
{
    gl_Position = projection * transform * vec4(in_position,1.0) ;
    out_color = in_color;
    out_texcoord = in_texcoord;
    out_normal = (transform * vec4(in_normal,0)).xyz;
}
