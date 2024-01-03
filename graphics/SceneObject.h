#pragma once

#include "../renderer/RenderObject.h"

/**
 * A 3D object data object. Vertices, materials, etc.
 */
struct SceneObject : public RenderObject {
};

SceneObject* LoadObject(const char* path);
