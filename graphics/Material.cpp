#include "Material.h"
#include <whb/file.h>
#include <whb/sdcard.h>

// TODO: stuff relating to rendering ought to be kept in renderer/
// and stuff relating to the scene objects more generally belong
// in graphics/

// generalized this a bit to be more like renderobject and allow different shaders easily, but also stuff like 
// textures and other things ~halcy

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

  // std::ifstream inVert;
  // std::ostringstream inStreamVert;
  
  // WHBLogPrintf("Loading shader %s",filename);
  // inVert.open(filename);
  // inStreamVert << inVert.rdbuf();
  // inVert.close();
  // destination.assign(inStreamVert.str());
}

