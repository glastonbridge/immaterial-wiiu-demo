#include "RenderBuffer.h"

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
#include <malloc.h>

#include "../util/ourmalloc.h"

RenderBuffer::~RenderBuffer() {
    free(texture.surface.image);
    free(depthBuffer.surface.image);
    free(contextState);
}

void RenderBuffer::renderUsing(const WHBGfxShaderGroup* group, int binding) {
    GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[binding].location);
    GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[binding].location);
    
    // Alpha off
    GX2SetBlendControl(GX2_RENDER_TARGET_0,
        GX2_BLEND_MODE_ONE, GX2_BLEND_MODE_ZERO,
        GX2_BLEND_COMBINE_MODE_ADD,
        FALSE,
        GX2_BLEND_MODE_ONE, GX2_BLEND_MODE_ZERO,
        GX2_BLEND_COMBINE_MODE_ADD
    );
}

RenderBuffer::RenderBuffer(bool highPrecision, int width_override, int height_override) {
    // Figure out sizes
    // we assume we want TV size. probably 1080p
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
    if (width_override > 0) {
        width = width_override;
    }
    if (height_override > 0) {
        height = height_override;
    }
    
    // Set up the texture first
    WHBLogPrintf("Setting up a render buffer with dimensions %i x %i", width, height);
    memset(&texture, 0, sizeof(GX2Texture));    
    texture.surface.use = GX2_SURFACE_USE_TEXTURE | GX2_SURFACE_USE_COLOR_BUFFER;
    texture.surface.width = width;
    texture.surface.height = height;
    texture.surface.depth = 1; // probably not relevant for 2D?
    texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D; // there's some fun ones like arrays or 3D textures if we want those
    if(highPrecision) {
        texture.surface.format = GX2_SURFACE_FORMAT_FLOAT_R32_G32_B32_A32;
    } else {
        texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
    }
    texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED; // probably memory layout stuff?
    texture.surface.aa = GX2_AA_MODE1X; // anti-aliasing off (but we could set it to on?)
    texture.viewNumSlices = 1; // I don't know what this means
    texture.compMap = 0x00010203; // This swizzles components I think?
    GX2CalcSurfaceSizeAndAlignment(&texture.surface);
    GX2InitTextureRegs(&texture);
    texture.surface.image = memalign(texture.surface.alignment, texture.surface.imageSize); // TODO maybe mem1 needed
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture.surface.image, texture.surface.imageSize);

    // Also set up a sampler
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR); // TODO can be point unless we do stupid tricks for cheap blur in shader

    // Color buffer
    memset(&colorBuffer, 0, sizeof(GX2ColorBuffer));    
    colorBuffer.surface = texture.surface;
    colorBuffer.viewNumSlices = 1;
    GX2InitColorBufferRegs(&colorBuffer);

    // Probably we'd also need a depth buffer
    memset(&depthBuffer, 0, sizeof(GX2DepthBuffer));
    depthBuffer.surface.use = GX2_SURFACE_USE_TEXTURE | GX2_SURFACE_USE_DEPTH_BUFFER; // maybe not okay
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
    depthBuffer.surface.image = memalign(depthBuffer.surface.alignment, depthBuffer.surface.imageSize);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, depthBuffer.surface.image, depthBuffer.surface.imageSize);

    // Set up a context state (the object that holds buffer info, what you actually bind to render to)
    contextState = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));
    GX2SetupContextStateEx(contextState, TRUE);
    GX2SetContextState(contextState);
    GX2SetColorBuffer(&colorBuffer, GX2_RENDER_TARGET_0);
    GX2SetDepthBuffer(&depthBuffer);
    GX2SetViewport(0, 0, (int)colorBuffer.surface.width, (int)colorBuffer.surface.height, 0.0f, 1.0f);
    GX2SetScissor(0, 0, (int)colorBuffer.surface.width, (int)colorBuffer.surface.height);
    GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, FALSE, TRUE);
    GX2SetBlendControl(GX2_RENDER_TARGET_0,
        GX2_BLEND_MODE_ONE, GX2_BLEND_MODE_ZERO,
        GX2_BLEND_COMBINE_MODE_ADD,
        FALSE,
        GX2_BLEND_MODE_ONE, GX2_BLEND_MODE_ZERO,
        GX2_BLEND_COMBINE_MODE_ADD
    );
    GX2SetDepthOnlyControl(TRUE, TRUE, GX2_COMPARE_FUNC_LESS);
}

void RenderBuffer::bindTarget(bool clear) {
    GX2ClearColor(&colorBuffer, 0.5, 0.4, 0.9, 1.0);
    GX2ClearDepthStencilEx(&depthBuffer, depthBuffer.depthClear, depthBuffer.stencilClear, GX2_CLEAR_FLAGS_DEPTH);
    GX2SetContextState(contextState);
}
void RenderBuffer::unbindTarget() {
    GX2DrawDone();
    GX2SetContextState(NULL);
    GX2Flush();
}
