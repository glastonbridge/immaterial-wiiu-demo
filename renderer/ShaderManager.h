
#include <map>
#include "RenderMaterial.h"
#include <coreinit/memdefaultheap.h>
#include <whb/file.h>
#include <whb/sdcard.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <gx2/shaders.h>
#include <cstdlib>

struct ShaderManager {
    ShaderManager();
    ~ShaderManager();

    WHBGfxShaderGroup* shader(const char* vsPath, const char* psPath, const std::vector<AttribSpec>& attribs);
    
    private:
        std::map<std::string, WHBGfxShaderGroup*> shaders;
};

ShaderManager* getShaderManager();
void destroyShaderManager();
