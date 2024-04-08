#pragma once
#include "../scenes/SceneBase.h"

// See scenes.cpp
SceneBase *getScene(int id);

struct RealScene : public SceneBase {
  void setup() final;
  void update(double time) final;
};

struct EepyScene : public SceneBase {
  void setup() final;
  void update(double time) final;
};
