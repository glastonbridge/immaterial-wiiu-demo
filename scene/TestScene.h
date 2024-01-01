
#include "SceneBase.h"

#include "Object.h"

struct TestScene: public SceneBase {
  void setup() override {
    
    objects.push_back(LoadObject(""));
  }

  void update(double time) override {

  }
};
