#version 450
layout(location=0) in vec4 in_color;
layout(location=1) in vec3 in_position;
uniform Projection {
    mat4 projection;
};
layout(location=0) out vec4 out_color;

void main()
{
    // Until the projection matrix can come through properly, use this sketchy
    gl_Position = projection * vec4(in_position,1.0);
    // If the out_color goes green, we are getting a value from projection matrix
    out_color = in_color;
}
