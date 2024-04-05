#pragma once

#include "../renderer/RenderMaterial.h"
#include "../renderer/RenderTexture.h"
#include <gx2/shaders.h>
#include <memory>
#include <vector>

struct SceneMaterial {
  SceneMaterial(){};
  virtual ~SceneMaterial(){};
  virtual RenderMaterial *getRenderMaterial() const {
    return renderMaterial.get();
  }

protected:
  std::unique_ptr<RenderMaterial> renderMaterial;
};
