
#include "SceneBase.h"

#include "../graphics/SceneObject.h"

struct TestScene: public SceneBase {
  void setup() override {
    
    objects.push_back(LoadObject("assets/cube.obj"));
  }

  void update(double time) override {

  }
};
