#pragma once

#include "../renderer/RenderBuffer.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

struct SceneInstance {
  size_t id;
  glm::mat4 transform;
  float anim;

  SceneInstance(size_t id) : id(id), transform(glm::mat4(1.0f)), anim(0.0f) {}
  SceneInstance(size_t id, glm::mat4 m) : id(id), transform(m), anim(0.0f) {}
};

struct SceneBase {
  std::vector<SceneInstance> instances;
  glm::mat4x4 cameraProjection;
  glm::mat4x4 cameraView;
  glm::vec4 cameraOptions;
  glm::vec4 processOptions;
  std::unique_ptr<RenderBuffer> renderBuffer;

  SceneBase()
    : renderBuffer(std::make_unique<RenderBuffer>(false, 1280, 720)) {};

  virtual void setup() = 0;
  virtual void update(double time) = 0;

  virtual ~SceneBase() = default;
};

// See scenes.cpp
SceneBase *getScene(int id);

// These don't really warrent their own headers
struct RealScene : public SceneBase {
  void setup() final;
  void update(double time) final;
};

struct EepyScene : public SceneBase {
  void setup() final;
  void update(double time) final;
};
