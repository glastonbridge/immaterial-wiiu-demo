#include "Spline.h"
#include <cmath>

glm::vec3 spline(SplineSegment const *seg, float t) {
  if (t <= 0.f) {
    return seg->pos;
  }

  auto f = std::floor(t);
  auto h = size_t(f);
  auto g = t - f;

  auto const &seg0 = seg[h];
  auto const &seg1 = seg[h+1];
  auto const x0 = seg0.pos;
  auto const x1 = seg0.pos + seg0.dir;
  auto const x2 = seg1.pos - seg1.dir;
  auto const x3 = seg1.pos;
  float ig = 1.0f - g;
  float ig2 = ig * ig;
  float ig3 = ig2 * ig;

  return ig3 * x0 + (g * 3.f) * (ig2 * x1 + ig * g * x2) + (g * g * g) * x3;
}

glm::vec3 splineDir(SplineSegment const *seg, float t) {
  if (t <= 0.f) {
    return 3.f * seg->dir;
  }

  auto f = std::floor(t);
  auto h = size_t(f);
  auto g = t - f;

  auto const &seg0 = seg[h];
  auto const &seg1 = seg[h+1];
  auto const x0 = seg0.pos;
  auto const x1 = seg0.pos + seg0.dir;
  auto const x2 = seg1.pos - seg1.dir;
  auto const x3 = seg1.pos;
  float ig = g - 1.f;

  float k0 = -3.f * (ig * ig);
  float k1 = 3.f * ig * (3.f * g - 1.f);
  float k2 = (6.f - 9.f * g) * g;

  return k0 * x0 + k1 * x1 + k2 * x2 + (3.f * g * g) * x3;
}
