#pragma once

#include "../graphics/SceneObject.h"
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

struct SceneBase {
  virtual void setup()=0;
  virtual void update(double time)=0;
  std::vector<std::unique_ptr<SceneObject>> objects;
  glm::mat4x4 cameraProjection;
};
