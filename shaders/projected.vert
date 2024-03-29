#version 450
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_normal;

layout(binding = 0) uniform Projection {
    mat4 projection;
};
layout(binding = 1) uniform Transform {
    mat4 transform;
};
layout(binding = 3) uniform View {
    mat4 view;
};
layout(binding = 4) uniform ExtraParams {
    vec4 extra_params;
};

layout(location = 0) out vec2 out_texcoord;
layout(location = 1) out vec3 out_pos;
layout(location = 2) out vec3 out_normal;
layout(location = 3) out vec3 out_pos_camspace;
layout(location = 4) out vec3 out_normal_camspace;
layout(location = 5) out vec4 out_cam_params;

void main()
{
    out_pos = (transform * vec4(in_position, 1.0f)).xyz;
    out_pos_camspace = (view * vec4(out_pos, 1.0f)).xyz;

    out_normal = (transform * vec4(in_normal, 0.0f)).xyz;
    out_normal_camspace = (view * vec4(out_normal, 0.0f)).xyz;

    gl_Position = projection * vec4(out_pos_camspace, 1.0);
    out_texcoord = in_texcoord;
    out_cam_params = extra_params;
}
