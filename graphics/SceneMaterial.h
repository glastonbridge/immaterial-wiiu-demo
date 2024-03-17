#pragma once

#include <memory>
#include <vector>
#include "../renderer/RenderMaterial.h"
#include "../renderer/RenderTexture.h"
#include <gx2/shaders.h>

struct SceneMaterial {
  SceneMaterial() {};
  virtual ~SceneMaterial() {};
  virtual RenderMaterial* getRenderMaterial() const {
    return renderMaterial.get();
  }

protected:
  std::unique_ptr<RenderMaterial> renderMaterial;
};
