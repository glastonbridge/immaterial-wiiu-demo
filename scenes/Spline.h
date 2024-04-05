#pragma once
#include <glm/glm.hpp>

// A SplineSegment consists of a position and a direction.
// The direction defines symmetric control points as offsets on either side of
// the position. At least two SplineSegment objects are required for a full
// spline but this allows an efficient implementation where all control points
// are symmetric.
//
// To use the spline functions, define an array of SplineSegments and pass in
// a pointer to the start of the array. Note there is no upper bounds check!
//
struct SplineSegment {
  glm::vec3 pos;
  glm::vec3 dir;
};

// Get the co-ordinates along the spline seg at position t
glm::vec3 spline(SplineSegment const *seg, float t);

// Get the derivative of the spline seg at position t
glm::vec3 splineDir(SplineSegment const *seg, float t);
