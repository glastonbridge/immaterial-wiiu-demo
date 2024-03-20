#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../graphics/SceneMaterial.h"

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
  std::vector<std::vector<glm::mat4>> animFrames;
  void setAnimationFrame(float frame);

protected:
  std::unique_ptr<RenderObject> renderObject;
  float* boneMatInterpBuffer = nullptr;
};

std::unique_ptr<SceneObject> LoadObject(const char* path, const char* name, SceneMaterial* material = nullptr);
std::unique_ptr<SceneObject> LoadQuad(SceneMaterial* material = nullptr);
