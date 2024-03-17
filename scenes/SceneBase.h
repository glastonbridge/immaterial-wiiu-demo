#pragma once

#include "../graphics/SceneObject.h"
#include "../renderer/RenderBuffer.h"
#include <glm/ext.hpp>

#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>


struct SceneBase {
  std::vector<std::unique_ptr<SceneObject>> objects;
  glm::mat4x4 cameraProjection;
  RenderBuffer* renderBuffer;

  SceneBase() {
    renderBuffer = new RenderBuffer();
  };
  
  void updateCamera() {
      cameraProjection = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 20.f) * \
      glm::lookAt(
        glm::vec3(0.0, -10.0, 0.0001), 
        glm::vec3(0.f, 0.f, 0.f), 
        glm::vec3(0.f, 1.f, 0.f)
      );
  }

  virtual void setup()=0;
  virtual void update(double time)=0;
  virtual void teardown()=0;

  virtual ~SceneBase() {
    delete renderBuffer;
  }
};
