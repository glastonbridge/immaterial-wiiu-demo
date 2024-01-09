#pragma once

#include "../renderer/CafeGLSLCompiler.h"

#include <gx2/shaders.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <lodepng.h>

#include <coreinit/memdefaultheap.h>
#include <gx2/mem.h>

struct WHBGfxShaderGroup;

// Helpers materials will need
WHBGfxShaderGroup *GLSL_CompileShader(const char *vsSrc, const char *psSrc);
void loadShader(const char* filename, std::string& destination);

struct Material {
  Material() {};
  ~Material() {};
  virtual void renderUsing() const {
    // Default impl, can be overridden if needed
    GX2SetFetchShader(&group->fetchShader);
    GX2SetVertexShader(group->vertexShader);
    GX2SetPixelShader(group->pixelShader);
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
  }

  protected: 
  WHBGfxShaderGroup * group;
};


// TODO copy these somewhere else
struct ProjectedMaterial: public Material {
  ProjectedMaterial() {
    group = {};

    std::string inStringVert;
    loadShader("shaders/projected.vert", inStringVert);
    const char* vertexProjected = inStringVert.c_str();

    std::string inStringFrag;
    loadShader("shaders/projected.frag", inStringFrag);
    const char* fragmentProjected = inStringFrag.c_str();

    group = GLSL_CompileShader(vertexProjected, fragmentProjected);
    if (!group) {
      WHBLogPrintf("Shader compilation failed");
    } else {
    
      WHBLogPrintf("Shader compilation completed");
    }

    WHBGfxInitShaderAttribute(group, "in_position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
    WHBGfxInitShaderAttribute(group, "in_color", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);

    WHBGfxInitFetchShader(group);
    
    WHBLogPrintf("Uniforms %u", group->vertexShader->uniformVarCount);
    WHBLogPrintf("Uniform Blocks %u", group->vertexShader->uniformBlockCount);
  }

  ~ProjectedMaterial() {
    WHBGfxFreeShaderGroup(group);
  }
};

struct BillboardMaterial: public Material {
  BillboardMaterial() {
    group = {};

    std::string inStringVert;
    loadShader("shaders/billboard.vert", inStringVert);
    const char* vertexBillboard = inStringVert.c_str();

    std::string inStringFrag;
    loadShader("shaders/billboard.frag", inStringFrag);
    const char* fragmentBillboard = inStringFrag.c_str();

    group = GLSL_CompileShader(vertexBillboard, fragmentBillboard);
    if (!group) {
      WHBLogPrintf("Shader compilation failed");
    } else {
    
      WHBLogPrintf("Shader compilation completed");
    }

    WHBGfxInitShaderAttribute(group, "in_position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
    WHBGfxInitShaderAttribute(group, "in_color", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
    WHBGfxInitFetchShader(group);
  }

  ~BillboardMaterial() {
    WHBGfxFreeShaderGroup(group);
  }
};


struct TextureMaterial: public Material {
  GX2Texture texture;
  GX2Sampler sampler;

  TextureMaterial(const char* path) {
    group = {};

    std::string inStringVert;
    loadShader("shaders/projected.vert", inStringVert);
    const char* vertexBillboard = inStringVert.c_str();

    std::string inStringFrag;
    loadShader("shaders/textured.frag", inStringFrag);
    const char* fragmentBillboard = inStringFrag.c_str();

    group = GLSL_CompileShader(vertexBillboard, fragmentBillboard);
    if (!group) {
      WHBLogPrintf("Shader compilation failed");
    } else {
    
      WHBLogPrintf("Shader compilation completed");
    }

    WHBGfxInitShaderAttribute(group, "in_position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
    WHBGfxInitShaderAttribute(group, "in_color", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
    WHBGfxInitShaderAttribute(group, "in_texcoord", 2, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
    WHBGfxInitFetchShader(group);

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
    } else {
      memcpy(texture.surface.image, image.data(), image.size());
    }
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture.surface.image, texture.surface.imageSize);
    
    // Also set up a sampler
    GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);
  }

  void renderUsing() const {
    Material::renderUsing();
    WHBLogPrintf("Rendering texture of size %u x %u", texture.surface.width, texture.surface.height);
    GX2SetPixelTexture(&texture, group->pixelShader->samplerVars[0].location);
    GX2SetPixelSampler(&sampler, group->pixelShader->samplerVars[0].location);
  }

  ~TextureMaterial() {
    WHBGfxFreeShaderGroup(group);
    MEMFreeToDefaultHeap(texture.surface.image);
  }
};
