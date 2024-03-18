#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_pos_camspace;
layout(location = 4) in vec3 in_normal_camspace;

layout(binding = 0) uniform sampler2D tex_sampler;

out vec4 out_color;

void main()
{
    vec3 ld = normalize(vec3(0.0f,-10.0f, 0.0f));
    vec3 n = normalize(in_normal);
    vec3 n_cam = normalize(in_normal_camspace);

    // coc calculation
    float focal_plane_dist = -10.0f;
    float dist_to_cam = length(in_pos_camspace.z);
    float dist_to_focal_plane = abs(in_pos_camspace.z - focal_plane_dist);
    float focal_length = 1.0f;
    float aperture = 0.05f;
    float coc_extent = dist_to_focal_plane / (dist_to_cam / (focal_length / aperture));

    out_color = texture(tex_sampler, in_texcoord) * vec4(vec3(0.5f + 0.5f * dot(ld, in_normal)), coc_extent * 0.05);

    vec3 to_camera = normalize(-in_pos_camspace);
    float fresnel = max(pow(1.0 - dot(to_camera, n_cam), 1.5), 0.0);
    out_color = out_color + vec4(fresnel, fresnel, fresnel, 0.0);

}
