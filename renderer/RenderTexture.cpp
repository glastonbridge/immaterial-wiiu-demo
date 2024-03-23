#include "RenderTexture.h"

#include <lodepng.h>
#include <vector>
#include <whb/log.h>
#include <coreinit/memdefaultheap.h>
#include <gx2/mem.h>
#include <gx2/clear.h>
#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/shaders.h>
#include <gx2/state.h>
#include <gx2/surface.h>
#include <gx2/swap.h>
#include <gx2/temp.h>
#include <gx2r/mem.h>
#include <stdlib.h>

#include "../util/ourmalloc.h"

RenderTexture::RenderTexture(const std::string& path) {
    // Load a texture
    std::vector<unsigned char> image;
    unsigned width, height;

    // Load data with lodepng
    unsigned error = lodepng::decode(image, width, height, path);
    if(error) {
      WHBLogPrintf("PNG load error on %s: %u: %s", path.c_str(), error, lodepng_error_text(error));
    }

    // Set up the texture
    WHBLogPrintf("Setting up a texture from %s: %i x %i", path.c_str(), width, height);
    memset(&texture, 0, sizeof(GX2Texture));
    texture.surface.use = GX2_SURFACE_USE_TEXTURE;
    texture.surface.width = width;
    texture.surface.height = height;
    texture.surface.depth = 1; // probably not relevant for 2D?
    texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D; // there's some fun ones like arrays or 3D textures if we want those
    texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8; // pixel format
    texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED; // probably memory layout stuff?
    texture.surface.aa = GX2_AA_MODE1X;
    texture.surface.mipLevels = 0;
    texture.surface.swizzle = 0;
    texture.compMap = 0x00010203; // This also swizzles components I think?
    texture.viewFirstMip = 0;
    texture.viewNumMips = 0;
    texture.viewFirstSlice = 0;
    texture.viewNumSlices = 1;
    GX2CalcSurfaceSizeAndAlignment(&texture.surface);
    GX2InitTextureRegs(&texture);
    WHBLogPrintf("Allocating %i bytes for texture", texture.surface.imageSize);
    texture.surface.image = memalign(texture.surface.alignment, texture.surface.imageSize);
    WHBLogPrintf("Allocated %p for texture", texture.surface.image);
    
    // Continuous to pitched if needed
    if(texture.surface.pitch != width) {
      uint32_t* texDataPtrPitched = (uint32_t*)texture.surface.image;
      uint32_t* imageDataPtrContinuous = (uint32_t*)image.data();
      for (int i = 0; i < texture.surface.height; i++) {
          memcpy((uint32_t*)&texDataPtrPitched[i * texture.surface.pitch], (uint32_t*)&imageDataPtrContinuous[i * width], width * sizeof(uint32_t));
      }
      WHBLogPrintf("Converted pitch and copied %i bytes to texture", width * sizeof(uint32_t));
    } else {
      memcpy(texture.surface.image, image.data(), image.size());
      WHBLogPrintf("Copied %i samples to texture", image.size());
    }
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture.surface.image, texture.surface.imageSize);
    
    // Also set up a sampler
    WHBLogPrintf("Setting up a texture sampler");
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);
}

RenderTexture::~RenderTexture() {
  WHBLogPrint("Freeing a texture");  
  free(texture.surface.image);
}

void RenderTexture::renderUsing(const WHBGfxShaderGroup* group, int binding) {
  if(binding < group->pixelShader->samplerVarCount) {
    // WHBLogPrintf("Binding a %p texture for group %p @ %i -> %i", this, group, binding, group->pixelShader->samplerVars[binding].location);
    GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[binding].location);
    GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[binding].location);
  }
}
