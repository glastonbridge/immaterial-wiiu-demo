#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_pos_camspace;

layout(binding = 0) uniform sampler2D tex_sampler;

out vec4 out_color;

void main()
{
    vec3 ld = normalize(vec3(0.0f,-10.0f, 0.0f));
    vec3 n = normalize(in_normal);
    //float coc_extent = pow(abs(in_pos_camspace.y) / 10.0, 2.0f) * 15.0f;

    float focal_plane_dist = -10.0f;
    float dist_to_cam = length(in_pos_camspace.z);
    float dist_to_focal_plane = abs(in_pos_camspace.z - focal_plane_dist);
    float focal_length = 1.0f;
    float aperture = 0.07f;
    float coc_extent = dist_to_focal_plane / (dist_to_cam / (focal_length / aperture));

    out_color = texture(tex_sampler, in_texcoord) * vec4(vec3(0.5f + 0.5f * dot(ld, in_normal)), coc_extent * 0.05);
}
