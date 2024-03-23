#version 450
layout(location = 0) in vec3 in_position;

layout(binding = 0) uniform Projection {
    mat4 projection;
};
layout(binding = 3) uniform View {
    mat4 view;
};

layout(location = 0) out vec3 out_texcoord;

void main()
{
    out_texcoord = in_position;  
    vec4 transformed_pos = projection * view * vec4(in_position, 0.0);
    transformed_pos.z = transformed_pos.w * 0.999;
    gl_Position = transformed_pos;
}
