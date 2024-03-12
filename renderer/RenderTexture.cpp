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

// crime
extern "C" {
    void* GfxHeapAllocMEM1(uint32_t size, uint32_t alignment);
    void GfxHeapFreeMEM1(void *block);
    void* GfxHeapAllocMEM2(uint32_t size, uint32_t alignment);
    void GfxHeapFreeMEM2(void *block);
}

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
  WHBLogPrint("Freeing a buffer");  
  MEMFreeToDefaultHeap(texture.surface.image);
}

void RenderTexture::renderUsing(const WHBGfxShaderGroup* group, int binding) {
  GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[binding].location);
  GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[binding].location);

}

/////////////////////////////////////////////////////

BufferTexture::~BufferTexture() {
    WHBLogPrint("Freeing a buffer");  
    MEMFreeToDefaultHeap(texture.surface.image);
    MEMFreeToDefaultHeap(depthBuffer.surface.image);
    GfxHeapFreeMEM2(contextState);
}

void BufferTexture::renderUsing(const WHBGfxShaderGroup* group, int binding) {
    GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[binding].location);
    GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[binding].location);
}

BufferTexture::BufferTexture() {
    // Figure out sizes
    int width, height;
    switch(GX2GetSystemTVScanMode()) {
        case GX2_TV_SCAN_MODE_480I:
        case GX2_TV_SCAN_MODE_480P:
            width = 854;
            height = 480;
        break;

        case GX2_TV_SCAN_MODE_1080I:
        case GX2_TV_SCAN_MODE_1080P:
            width = 1920;
            height = 1080;
        break;

        case GX2_TV_SCAN_MODE_720P:
        default:
            width = 1280;
            height = 720;
        break;
    }

    // Set up the texture first
    texture.surface.use = GX2_SURFACE_USE_TEXTURE | GX2_SURFACE_USE_COLOR_BUFFER;
    texture.surface.width = width;
    texture.surface.height = height;
    texture.surface.depth = 1; // probably not relevant for 2D?
    texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D; // there's some fun ones like arrays or 3D textures if we want those
    texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8; // pixel format
    texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED; // probably memory layout stuff?
    texture.surface.aa = GX2_AA_MODE1X; // anti-aliasing off (but we could set it to on?)
    texture.viewNumSlices = 1; // I don't know what this means
    texture.compMap = 0x00010203; // This swizzles components I think?
    GX2CalcSurfaceSizeAndAlignment(&texture.surface);
    GX2InitTextureRegs(&texture);
    texture.surface.image = MEMAllocFromDefaultHeapEx(texture.surface.imageSize, texture.surface.alignment); // TODO maybe mem1 needed
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture.surface.image, texture.surface.imageSize);

    // Also set up a sampler
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);

    // Color buffer
    colorBuffer.surface = texture.surface;
    colorBuffer.viewNumSlices = 1;
    GX2InitColorBufferRegs(&colorBuffer);

    // Probably we'd also need a depth buffer
    depthBuffer.surface.use = GX2_SURFACE_USE_TEXTURE | GX2_SURFACE_USE_COLOR_BUFFER;
    depthBuffer.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
    depthBuffer.surface.width = width;
    depthBuffer.surface.height = height;
    depthBuffer.surface.depth = 1;
    depthBuffer.surface.mipLevels = 1;
    depthBuffer.surface.format = GX2_SURFACE_FORMAT_FLOAT_R32;
    depthBuffer.surface.aa = GX2_AA_MODE1X;
    depthBuffer.surface.tileMode = GX2_TILE_MODE_DEFAULT;
    depthBuffer.viewNumSlices = 1;
    depthBuffer.depthClear = 1.0f;
    GX2CalcSurfaceSizeAndAlignment(&(depthBuffer.surface));
    GX2InitDepthBufferRegs(&depthBuffer);
    depthBuffer.surface.image = MEMAllocFromDefaultHeapEx(depthBuffer.surface.imageSize, depthBuffer.surface.alignment);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, depthBuffer.surface.image, depthBuffer.surface.imageSize);

    contextState = (GX2ContextState*)GfxHeapAllocMEM2(sizeof(GX2ContextState), GX2_CONTEXT_STATE_ALIGNMENT);
    GX2SetupContextStateEx(contextState, TRUE);
    GX2SetContextState(contextState);
    GX2SetColorBuffer(&colorBuffer, GX2_RENDER_TARGET_0);
    GX2SetDepthBuffer(&depthBuffer);
    GX2SetViewport(0, 0, (float)colorBuffer.surface.width, (float)colorBuffer.surface.height, 0.0f, 1.0f);
    GX2SetScissor(0, 0, (float)colorBuffer.surface.width, (float)colorBuffer.surface.height);
}

void BufferTexture::bindTarget(bool clear) {
    GX2ClearColor(&colorBuffer, 0.5, 0.4, 0.9, 1.0);
    GX2ClearDepthStencilEx(&depthBuffer, depthBuffer.depthClear, depthBuffer.stencilClear, GX2_CLEAR_FLAGS_DEPTH);
    GX2SetContextState(contextState);
}
void BufferTexture::unbindTarget() {
    GX2SetContextState(NULL);
}

