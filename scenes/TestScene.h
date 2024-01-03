
#include "SceneBase.h"

#include "../graphics/SceneObject.h"

struct TestScene: public SceneBase {
  void setup() override {
    
    objects.push_back(LoadObject(""));
  }

  void update(double time) override {

  }
};
