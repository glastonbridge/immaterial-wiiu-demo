#include "RenderTexture.h"

#include <lodepng.h>
#include <vector>
#include <whb/log.h>
#include <coreinit/memdefaultheap.h>
#include <gx2/mem.h>

RenderTexture::RenderTexture(const std::string& path) {

    // Load a texture
    std::vector<unsigned char> image;
    unsigned width, height;

    // Load data with lodepng
    unsigned error = lodepng::decode(image, width, height, path);
    if(error) {
      WHBLogPrintf("PNG load error %u: %s", error, lodepng_error_text(error));
    }

    // Set up the texture
    texture.surface.width = width;
    texture.surface.height = height;
    texture.surface.depth = 1; // probably not relevant for 2D?
    texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D; // there's some fun ones like arrays or 3D textures if we want those
    texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8; // pixel format
    texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED; // probably memory layout stuff?
    texture.viewNumSlices = 1; // I don't know what this means
    texture.compMap = 0x00010203; // This swizzles components I think?
    GX2CalcSurfaceSizeAndAlignment(&texture.surface);
    GX2InitTextureRegs(&texture);
    texture.surface.image = MEMAllocFromDefaultHeapEx(texture.surface.imageSize, texture.surface.alignment);

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
      WHBLogPrintf("Converted pitch and copied %i samples to texture", image.size());
    }
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture.surface.image, texture.surface.imageSize);
    
    // Also set up a sampler
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);
}

RenderTexture::~RenderTexture() {
  WHBLogPrint("Freeing a texture");  
  MEMFreeToDefaultHeap(texture.surface.image);
}

void RenderTexture::renderUsing(const WHBGfxShaderGroup* group) {
  GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[0].location);
  GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[0].location);

}
