#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_pos_camspace;
layout(location = 4) in vec3 in_normal_camspace;
layout(location = 5) in vec4 in_cam_params;

layout(binding = 0) uniform sampler2D tex_sampler;

out vec4 out_color;

void main()
{
    vec3 ld = normalize(vec3(0.0f, 10.0f, 0.0f));
    vec3 n = normalize(in_normal);
    vec3 n_cam = normalize(in_normal_camspace);

    // coc calculation
    float focal_plane_dist = in_cam_params.x;
    float dist_to_cam = length(in_pos_camspace.z);
    float dist_to_focal_plane = abs(in_pos_camspace.z - focal_plane_dist);
    float focal_length = in_cam_params.y;
    float aperture = in_cam_params.z;
    float coc_extent = dist_to_focal_plane / (dist_to_cam / (focal_length / aperture));
    coc_extent = clamp(coc_extent, 0.0, 1.0);
    vec4 tex_color = texture(tex_sampler, in_texcoord);
    tex_color.a = 1.0;
    out_color = tex_color * vec4(vec3(0.5f + 0.5f * dot(ld, in_normal)), coc_extent);
    // out_color = vec4(1.0) * vec4(vec3(0.5f + 0.5f * dot(ld, in_normal)), coc_extent);
    vec3 to_camera = normalize(-in_pos_camspace);
    float fresnel = min(max(pow(1.0 - dot(to_camera, n_cam), in_cam_params.w), 0.0), 1.0);
    out_color = mix(out_color, vec4(1.0, 1.0, 1.0, 0.0), fresnel);

}
