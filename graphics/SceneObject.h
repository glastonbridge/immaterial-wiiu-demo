#pragma once

#include <memory>

struct RenderObject;

/**
 * A 3D object data object. Vertices, materials, etc.
 */
struct SceneObject {
  SceneObject();
  ~SceneObject();
  RenderObject* getRenderObject();
protected:
  std::unique_ptr<RenderObject> renderObject;
};

std::unique_ptr<SceneObject> LoadObject(const char* path);
