#pragma once


#include <gx2/texture.h>
#include <gx2/shaders.h>
#include <string>
#include <whb/gfx.h>

/**
 * A texture, typically owned by a RenderMaterial
 */
struct RenderTexture {
  RenderTexture(const std::string& path);
  ~RenderTexture();
  void renderUsing(const WHBGfxShaderGroup* group);
private:  
  GX2Texture texture;
  GX2Sampler sampler;
};
