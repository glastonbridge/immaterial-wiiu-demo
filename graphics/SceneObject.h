#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct RenderObject;

/**
 * A 3D object data object. Vertices, materials, etc.
 * 
 * Ideally, you manipulate this in scenes, and don't need to touch the renderobject.
 * Separation of concerns unlocked. In practice: lol. lmao.
 */
struct SceneObject {
  SceneObject();
  ~SceneObject();
  RenderObject* getRenderObject();
  std::vector<std::vector<glm::mat4x3>> animFrames;

protected:
  std::unique_ptr<RenderObject> renderObject;
};

std::unique_ptr<SceneObject> LoadObject(const char* path);
