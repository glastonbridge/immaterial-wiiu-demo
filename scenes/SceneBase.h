#pragma once

#include "../graphics/SceneObject.h"
#include "../graphics/SceneMaterial.h"
#include "../renderer/RenderObject.h"
#include "../renderer/RenderBuffer.h"
#include "../sync/Sync.h"

#include <glm/ext.hpp>

#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

struct SceneInstance {
  size_t id;
  glm::mat4 transform;
  float anim;

  SceneInstance(size_t id) : id(id), transform(glm::mat4(1.0f)), anim(0.0f) {}
  SceneInstance(size_t id, glm::mat4 m) : id(id), transform(m), anim(0.0f) {}
};

struct SceneBase {
  std::vector<std::unique_ptr<SceneObject>> objects;
  std::vector<std::unique_ptr<SceneMaterial>> materials;
  std::vector<SceneInstance> instances;
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
