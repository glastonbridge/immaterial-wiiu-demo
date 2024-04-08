#pragma once

#include "RenderObject.h"
#include <gx2/enum.h>
#include <memory>
#include <string>
#include <vector>

struct WHBGfxShaderGroup;
struct RenderTexture;

struct AttribSpec {
  std::string name;
  BufferType type;
  GX2AttribFormat format;
};

struct RenderMaterial {
  RenderMaterial(const std::string &vertexShaderPath,
                 const std::string &fragmentShaderPath,
                 const std::vector<AttribSpec> &attribs);

  virtual ~RenderMaterial();
  virtual void renderUsing() const;
  void setTexture(std::unique_ptr<RenderTexture> texture);
  const int getBindingForBuffer(BufferType bt) const {
    return this->bindingForBuffer[bt];
  }

  // protected:
  WHBGfxShaderGroup *group;
  std::unique_ptr<RenderTexture> texture;
  int bindingForBuffer[BufferType::_MAX];
};
