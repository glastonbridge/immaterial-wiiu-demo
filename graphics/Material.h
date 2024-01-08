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

struct WHBGfxShaderGroup;

// Helpers materials will need
WHBGfxShaderGroup *GLSL_CompileShader(const char *vsSrc, const char *psSrc);
void loadShader(const char* filename, std::string& destination);

struct Material {
  Material() {};
  ~Material() {};
  void renderUsing() const;
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
    // TODO: destroy shaders, if we care about that
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
    // TODO: destroy shaders, if we care about that
  }
};
