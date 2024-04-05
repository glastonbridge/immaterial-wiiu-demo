
#include "RenderMaterial.h"
#include <coreinit/memdefaultheap.h>
#include <cstdlib>
#include <gx2/shaders.h>
#include <unordered_map>
#include <whb/file.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <whb/sdcard.h>

struct ShaderManager {
  ShaderManager();
  ~ShaderManager();

  WHBGfxShaderGroup *shader(const char *vsPath, const char *psPath,
                            const std::vector<AttribSpec> &attribs);

private:
  std::unordered_map<std::string, WHBGfxShaderGroup *> shaders;
};

ShaderManager *getShaderManager();
void destroyShaderManager();
