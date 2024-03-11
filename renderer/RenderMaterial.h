#pragma once

#include <string>
#include <vector>
#include <memory>
#include <gx2/enum.h>
#include "RenderTexture.h"
#include "RenderObject.h"

struct WHBGfxShaderGroup;

struct AttribSpec {
  std::string name;
  BufferType type;
  GX2AttribFormat format;
};

struct RenderMaterial {
  RenderMaterial(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const std::vector<AttribSpec>& 
  );
  
  virtual ~RenderMaterial();
  virtual void renderUsing() const;
  void setTexture(RenderTexture* texture);
  const int getBindingForBuffer(BufferType bt) const {
    return this->bindingForBuffer[bt];
  }

  protected: 
  WHBGfxShaderGroup * group;
  std::unique_ptr<RenderTexture> texture;
  int bindingForBuffer[BufferType::_MAX];
};

