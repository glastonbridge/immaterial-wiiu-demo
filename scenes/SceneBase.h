#pragma once

#include "../graphics/SceneObject.h"
#include "../renderer/RenderObject.h"
#include "../renderer/RenderBuffer.h"
#include "../sync/Sync.h"

#include <glm/ext.hpp>

#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

struct SceneBase {
  std::vector<std::unique_ptr<SceneObject>> objects;
  glm::mat4x4 cameraProjection;
  glm::mat4x4 cameraView;
  RenderBuffer* renderBuffer;

  SceneBase() {
    renderBuffer = new RenderBuffer();
  };
  
  void updateCamera() {
      cameraProjection = glm::perspective(glm::radians(syncVal("Camera:FoV")), 1920.0f/1080.0f, 0.1f, 2000.f);
      cameraView = glm::lookAt(
        glm::vec3(syncVal("Camera:PosX"), syncVal("Camera:PosY"), syncVal("Camera:PosZ")),
        glm::vec3(syncVal("Camera:LookatX"), syncVal("Camera:LookatY"), syncVal("Camera:LookatZ")),
        glm::vec3(syncVal("Camera:UpX"), syncVal("Camera:UpY"), syncVal("Camera:UpZ"))
      );
  }

  virtual void setup()=0;
  virtual void update(double time)=0;
  virtual void teardown()=0;

  virtual ~SceneBase() {
    delete renderBuffer;
  }
};
