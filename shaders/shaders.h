#pragma once
const char *vertexShader = "\
#version 450\n\
layout(location=0) in vec4 in_color;\n\
layout(location=1) in vec4 in_position;\n\
layout(location=0) out vec4 out_color;\n\
void main()\n\
{\n\
    gl_Position = in_position;\n\
    out_color = in_color;\n\
}\n\
";
const char *fragmentShader = "\
#version 450\n\
layout(location=0) in vec4 in_color;\n\
out vec4 out_color;\n\
void main()\n\
{\n\
    out_color = in_color;\n\
}\n\
";
