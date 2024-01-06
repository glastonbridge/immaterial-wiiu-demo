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
    // hard-coded projection matrix
    mat4 miniproj = mat4(
        0.822204,0.000000,0.000000,0.000000,
        0.000000,1.461696,0.000000,0.000000,
        0.000000,0.000000,-1.000100,-1.000000,
        0.000000,0.000000,-0.200010,1.000000
    );
    gl_Position = miniproj * vec4(in_position,1.0);
    // If the out_color goes green, we are getting a value from projection matrix
    out_color = vec4(0.0,projection[0].x,0.0,1.0);//in_color;
}
