#pragma once

#include <glm/glm.hpp>

/**
 * A 3D object data object. Vertices, materials, etc.
 *
 * Ideally, you manipulate this in scenes, and don't need to touch the
 * renderobject. Separation of concerns unlocked. In practice: lol. lmao.
 */
struct SceneObject {
  SceneObject(size_t id) : id(id), transform(glm::mat4(1.0f)), anim(0.0f) {}
  SceneObject(size_t id, glm::mat4 m) : id(id), transform(m), anim(0.0f) {}

  size_t id;
  glm::mat4 transform;
  float anim;
};
