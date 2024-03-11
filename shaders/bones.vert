#version 450

layout(location=0) in vec3 in_position;
layout(location=1) in vec2 in_texcoord;
layout(location=2) in vec3 in_normal;

layout(location=3) in ivec2 in_bone_idx;
layout(location=4) in vec2 in_bone_weight;

layout(binding=0) uniform Projection {
    mat4 projection;
};
layout(binding=1) uniform Transform {
    mat4 transform;
};
layout(binding=2) uniform BoneTransform {
    mat4x3 bone_transform[32]; // 32 bones should be enough for everyone
};

layout(location=0) out vec2 out_texcoord;
layout(location=1) out vec3 out_normal;
void main()
{
    mat4x3 bone_mat = bone_transform[in_bone_idx[0]] * in_bone_weight[0] + bone_transform[in_bone_idx[1]] * in_bone_weight[1];
    vec3 vert_skinned = bone_mat * vec4(in_position, 1.0);
    gl_Position = projection * transform * vec4(vert_skinned, 1.0);
    out_texcoord = in_texcoord;
    out_normal = (transform * vec4(in_normal, 0)).xyz;
}
