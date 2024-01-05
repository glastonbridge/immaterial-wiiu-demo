#pragma once

#include "../renderer/RenderObject.h"
#include <memory>

/**
 * A 3D object data object. Vertices, materials, etc.
 */
struct SceneObject : public RenderObject {
};

std::unique_ptr<SceneObject> LoadObject(const char* path);
