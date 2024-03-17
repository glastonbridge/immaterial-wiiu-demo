#pragma once

#include <gx2/texture.h>
#include <gx2/shaders.h>
#include <string>
#include <whb/gfx.h>

/**
 * A texture that is also a buffer that can be rendered to
*/
struct RenderBuffer {
  RenderBuffer();
  ~RenderBuffer();

  void bindTarget(bool clear = true);
  void unbindTarget();
  void renderUsing(const WHBGfxShaderGroup* group, int binding = 0);

private:  
  GX2Texture texture;
  GX2Sampler sampler;
  GX2ColorBuffer colorBuffer;
  GX2DepthBuffer depthBuffer;
  GX2ContextState* contextState;
};
