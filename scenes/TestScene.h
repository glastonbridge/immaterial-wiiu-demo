#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../renderer/RenderObject.h"

#include "../graphics/SceneObject.h"

struct TestScene: public SceneBase {
  void setup() override {
    objects.push_back(LoadObject("assets/train.fbx", NULL));
  }

  void update(double time) override {
    updateCamera();

    auto transform = glm::rotate(glm::mat4(1.f), glm::radians(syncVal("TestPart:Object:RotY")), glm::vec3(0.f, 1.f, 0.f));
    
    float* mat = (float*)glm::value_ptr(transform);
    objects[0]->getRenderObject()->setUniformFloatMat(UniformType::TRANSFORM, mat, 16);
    objects[0]->setAnimationFrame(syncVal("TestPart:Object:Frame"));
  }

  void teardown() override {
    // nothing to do
  }
};
