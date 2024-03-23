#pragma once


#include <gx2/texture.h>
#include <gx2/shaders.h>
#include <string>
#include <whb/gfx.h>

/**
 * A texture, typically owned by a RenderMaterial
 */
struct RenderTexture {
  RenderTexture(const std::string& path, bool isCubeMap = false);
  ~RenderTexture();
  void renderUsing(const WHBGfxShaderGroup* group, int binding = 0);
private:  
  GX2Texture texture;
  GX2Sampler sampler;
};

