#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

void calculateProjectionMatrix(
  float* matrix, float fov, float aspect, float znear, float zfar) {

    float ymax = (float) (znear * tanf(fov));
    float xmax = ymax * aspect;

    float twoZNear = 2.0f * znear;
    float deltaW = 2.0f*xmax;
    float deltaH = 2.0f*ymax;
    float deltaZ= zfar - znear;
    matrix[0] = twoZNear / deltaW;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;
    matrix[4] = 0.0f;
    matrix[5] = twoZNear / deltaH;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;
    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = (-zfar - znear) / deltaZ;
    matrix[11] = -1.0f;
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = (-twoZNear * zfar) / deltaZ;
    matrix[15] = 1.0f;
}
