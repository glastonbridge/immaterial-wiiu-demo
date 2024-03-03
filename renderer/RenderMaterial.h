#pragma once

#include <string>
#include <vector>
#include <memory>
#include <gx2/enum.h>
#include "RenderTexture.h"

struct WHBGfxShaderGroup;

struct AttribSpec {
  std::string name;
  GX2AttribFormat format;
};

struct RenderMaterial {
  RenderMaterial(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const std::vector<AttribSpec>& 
  );
  virtual ~RenderMaterial();
  virtual void renderUsing() const ;
  void setTexture(RenderTexture* texture);

  protected: 
  WHBGfxShaderGroup * group;
  std::unique_ptr<RenderTexture> texture;
};

