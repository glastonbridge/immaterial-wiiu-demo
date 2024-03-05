#include "RenderMaterial.h"
#include <coreinit/memdefaultheap.h>
#include <whb/file.h>
#include <whb/sdcard.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <gx2/shaders.h>
#include "../renderer/CafeGLSLCompiler.h"
#include <cstdlib>

// Helpers
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
  // TODO: these WHB file operations are deprecated and should have been replaced with standard C++
  // file operations. But using the C++ file operations somehow causes our app to not quit properly
  // and the console crashes when trying to quit to menu. Why??

  char *sdRootPath = WHBGetSdCardMountPath();
  char path[256];
  sprintf(path, "%s/%s", sdRootPath, filename);
  WHBLogPrintf("Loading shader %s", path);
  char *text = WHBReadWholeFile(path, NULL);
  destination = text;
}

RenderMaterial::RenderMaterial(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const std::vector<AttribSpec>& attribs
  ) {
      group = {};

      std::string inStringVert;
      loadShader(vertexShaderPath.c_str(), inStringVert);
      const char* vertexProjected = inStringVert.c_str();

      std::string inStringFrag;
      loadShader(fragmentShaderPath.c_str(), inStringFrag);
      const char* fragmentProjected = inStringFrag.c_str();

      group = GLSL_CompileShader(vertexProjected, fragmentProjected);
      if (!group) {
        WHBLogPrintf("Shader compilation failed");
      } else {
      
        WHBLogPrintf("Shader compilation completed");
      }
      WHBLogPrintf("Shaders loaded %s %s", vertexShaderPath.c_str(), fragmentShaderPath.c_str());
      for (uint32_t i = 0; i < attribs.size(); ++i) {
         WHBLogPrintf("Attaching attributes %s - %i", attribs[i].name.c_str(), attribs[i].format);
         WHBGfxInitShaderAttribute(group, attribs[i].name.c_str(), i, 0, attribs[i].format);
      }

      WHBGfxInitFetchShader(group);
}

void RenderMaterial::renderUsing() const {
    GX2SetFetchShader(&group->fetchShader);
    GX2SetVertexShader(group->vertexShader);
    GX2SetPixelShader(group->pixelShader);
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
    if (texture) {
      texture->renderUsing(group);
    }
}

void RenderMaterial::setTexture(RenderTexture* _texture) {
   texture.reset(_texture);
}

RenderMaterial::~RenderMaterial() {
    GLSL_FreeVertexShader(group->vertexShader);
    GLSL_FreePixelShader(group->pixelShader);
    free(group);
}
