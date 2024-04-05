#pragma once

#include "../graphics/SceneObject.h"
#include "../renderer/RenderBuffer.h"
#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

struct SceneBase {
  std::vector<SceneObject> instances;
  glm::mat4x4 cameraProjection;
  glm::mat4x4 cameraView;
  glm::vec4 cameraOptions;
  glm::vec4 processOptions;
  std::unique_ptr<RenderBuffer> renderBuffer;

  SceneBase()
      : renderBuffer(std::make_unique<RenderBuffer>(false, 1280, 720)){};

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
