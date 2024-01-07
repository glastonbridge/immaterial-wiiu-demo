#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

void calculateProjectionMatrix(
  float* matrix, float fov, float aspect, float znear, float zfar);