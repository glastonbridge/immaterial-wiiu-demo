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

RenderTexture::RenderTexture(const std::string& path, bool isCubeMap) {
    // Load a texture
    std::vector<unsigned char> image;
    unsigned width, height;

    // Load data with lodepng
    if(!isCubeMap) {
      unsigned error = lodepng::decode(image, width, height, path);
      
      WHBLogPrintf("Setting up a texture from %s: %i x %i", path.c_str(), width, height);
      if(error) {
        WHBLogPrintf("PNG load error on %s: %u: %s", path.c_str(), error, lodepng_error_text(error));
      }
    } else {
      std::vector<unsigned char> image_load;
      std::vector<std::string> faces{
          "px.png",
          "nx.png",
          "py.png",
          "ny.png",
          "pz.png",
          "nz.png"
      };
      for(int i = 0; i < 6; i++) {
        std::string cubeMapPath = path + faces[i];
        image_load.clear();
        unsigned error = lodepng::decode(image_load, width, height, cubeMapPath);
        WHBLogPrintf("Setting up a cube face texture from %s: %i x %i", path.c_str(), width, height);
        if(error) {
          WHBLogPrintf("PNG load error on %s: %u: %s", cubeMapPath.c_str(), error, lodepng_error_text(error));
        }
        for (int i = 0; i < image_load.size(); i++) {
            image.push_back(image_load[i]);
        }
      }
    }

    // Set up the texture
    memset(&texture, 0, sizeof(GX2Texture));
    texture.surface.use = GX2_SURFACE_USE_TEXTURE;
    texture.surface.width = width;
    texture.surface.height = height;
    if(!isCubeMap) {
      texture.surface.depth = 1; // probably not relevant for 2D?
      texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D; // there's some fun ones like arrays or 3D textures if we want those
    }
    else {
      texture.surface.depth = 6;
      texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_CUBE;
    }
    texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8; // pixel format
    texture.surface.tileMode = GX2_TILE_MODE_DEFAULT; // GX2_TILE_MODE_LINEAR_ALIGNED; // probably memory layout stuff?
    texture.surface.aa = GX2_AA_MODE1X;
    texture.surface.mipLevels = 0;
    texture.surface.swizzle = 0;
    texture.compMap = 0x00010203; // This also swizzles components I think?
    texture.viewFirstMip = 0;
    texture.viewNumMips = 0;
    texture.viewFirstSlice = 0;
    if(!isCubeMap) {    
      texture.viewNumSlices = 1;
    }
    else {
      texture.viewNumSlices = 6;
    }
    GX2CalcSurfaceSizeAndAlignment(&texture.surface);
    GX2InitTextureRegs(&texture);
    WHBLogPrintf("Allocating %i bytes for texture", texture.surface.imageSize);
    texture.surface.image = memalign(texture.surface.alignment, texture.surface.imageSize);
    WHBLogPrintf("Allocated %p for texture", texture.surface.image);
    
    // tile-zorder the image
    // if you use nonsquare or not power of two images this breaks
    uint32_t* imageDataPtrLinear = (uint32_t*)image.data();
    GX2Surface tempSurface = texture.surface;
    tempSurface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
    GX2CalcSurfaceSizeAndAlignment(&tempSurface);
    tempSurface.image = memalign(tempSurface.alignment, tempSurface.imageSize);

    // Continuous to pitched if needed - commented out, we now use tiled
    if(tempSurface.pitch != width) {
      WHBLogPrintf("Converting pitch and copying %i bytes to texture", width * sizeof(uint32_t));
      uint32_t* texDataPtrPitched = (uint32_t*)tempSurface.image;
      uint32_t* imageDataPtrContinuous = (uint32_t*)image.data();
      for (int i = 0; i < tempSurface.height; i++) {
          memcpy((uint32_t*)&texDataPtrPitched[i * tempSurface.pitch], (uint32_t*)&imageDataPtrContinuous[i * width], width * sizeof(uint32_t));
      }
    } else {
      WHBLogPrintf("Copyping %i samples to texture", image.size());
      memcpy(tempSurface.image, image.data(), image.size());
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, tempSurface.image, tempSurface.imageSize);
    GX2CopySurface(&tempSurface, 0, 0, &texture.surface, 0, 0);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture.surface.image, texture.surface.imageSize);
    GX2Flush();

    // Also set up a sampler
    WHBLogPrintf("Setting up a texture sampler");
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);

    // clean up image data
    free(tempSurface.image);
}

RenderTexture::~RenderTexture() {
  WHBLogPrint("Freeing a texture");  
  free(texture.surface.image);
}

void RenderTexture::renderUsing(const WHBGfxShaderGroup* group, int binding) {
  if(binding < group->pixelShader->samplerVarCount) {
    //WHBLogPrintf("Binding a %p texture for group %p @ %i -> %i / %i", this, group, binding, group->pixelShader->samplerVars[binding].location, group->pixelShader->samplerVarCount);
    GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[binding].location);
    GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[binding].location);
  }
}