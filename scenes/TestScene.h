#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../renderer/RenderObject.h"

#include "../graphics/SceneObject.h"

struct TestScene: public SceneBase {
  void setup() override {
    
    objects.push_back(LoadObject("assets/cube.obj"));
  }

  void update(double time) override {
    cameraProjection = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 20.f) * \
      glm::translate(glm::mat4(1.f), glm::vec3(-0.7f, -0.7f, -10.f));

    auto transform = glm::rotate(glm::mat4(1.f), glm::radians(syncVal("TestPart:Object:RotY")), glm::vec3(0.f, 1.f, 0.f));
    
    // This bit isn't working
    float* mat = (float*)glm::value_ptr(transform);
    objects[0]->getRenderObject()->setUniformFloatMat(UniformType::TRANSFORM, mat, 16);

    // So slap it on the camera projection for now
    cameraProjection *= transform;
  }
};
