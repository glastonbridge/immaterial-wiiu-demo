#pragma once

#include <vector>

#include "../graphics/SceneObject.h"

struct SceneBase {
  virtual void setup()=0;
  virtual void update(double time)=0;
  std::vector<SceneObject*> objects;
};
