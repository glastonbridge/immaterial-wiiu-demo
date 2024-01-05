#include "Material.h"

#include "../renderer/CafeGLSLCompiler.h"

#include <gx2/shaders.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>



WHBGfxShaderGroup *GLSL_CompileShader(const char *vsSrc, const char *psSrc)
{
   char infoLog[1024];
   GX2VertexShader *vs = GLSL_CompileVertexShader(vsSrc, infoLog, sizeof(infoLog), GLSL_COMPILER_FLAG_NONE);
   if (!vs)
   {
      OSReport("Failed to compile vertex shader. Infolog: %s\n", infoLog);
      return NULL;
   }
   GX2PixelShader *ps = GLSL_CompilePixelShader(psSrc, infoLog, sizeof(infoLog), GLSL_COMPILER_FLAG_NONE);
   if (!ps)
   {
      OSReport("Failed to compile pixel shader. Infolog: %s\n", infoLog);
      return NULL;
   }
   WHBGfxShaderGroup *shaderGroup = (WHBGfxShaderGroup *)malloc(sizeof(WHBGfxShaderGroup));
   memset(shaderGroup, 0, sizeof(*shaderGroup));
   shaderGroup->vertexShader = vs;
   shaderGroup->pixelShader = ps;
   return shaderGroup;
}

void loadShader(const char* filename, std::string& destination) {
  std::ifstream inVert;
  std::ostringstream inStreamVert;
  inVert.open(filename);
  inStreamVert << inVert.rdbuf();
  inVert.close();
  destination.assign(inStreamVert.str());
}

void Material::attachBillboardShaders() {
   group = {};

   

  std::string inStringVert;
  loadShader("billboard.vert", inStringVert);
  const char* vertexBillboard = inStringVert.c_str();

  std::string inStringFrag;
  loadShader("billboard.frag", inStringFrag);
  const char* fragmentBillboard = inStringFrag.c_str();

  // TODO: Attach different shaders to different materials
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


void Material::attachPerspectiveShaders() {
   group = {};
  // TODO: Attach different shaders to different materials

  std::string inStringVert;
  loadShader("projected.vert", inStringVert);
  const char* vertexProjected = inStringVert.c_str();

  std::string inStringFrag;
  loadShader("projected.frag", inStringFrag);
  const char* fragmentProjected = inStringFrag.c_str();

  group = GLSL_CompileShader(vertexProjected, fragmentProjected);
  if (!group) {
    WHBLogPrintf("Shader compilation failed");
  } else {
   
    WHBLogPrintf("Shader compilation completed");
  }

  WHBGfxInitShaderAttribute(group, "in_position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
  WHBGfxInitShaderAttribute(group, "in_color", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
  WHBGfxInitShaderAttribute(group, "projection", 2, 0, GX2_ATTRIB_FORMAT_FLOAT_32);
  WHBGfxInitFetchShader(group);
}

void Material::renderUsing() const {
  GX2SetFetchShader(&group->fetchShader);
  GX2SetVertexShader(group->vertexShader);
  GX2SetPixelShader(group->pixelShader);
}
