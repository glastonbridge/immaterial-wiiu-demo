#include "RenderMaterial.h"
#include <coreinit/memdefaultheap.h>
#include <whb/file.h>
#include <whb/sdcard.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <gx2/shaders.h>
#include "../renderer/CafeGLSLCompiler.h"
#include <cstdlib>
#include "ShaderManager.h"

RenderMaterial::RenderMaterial(
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    const std::vector<AttribSpec>& attribs
) {
    group = getShaderManager()->shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), attribs);
    for (uint32_t i = 0; i < BufferType::_MAX; ++i) {
        bindingForBuffer[i] = -1;
    }
    for (uint32_t i = 0; i < attribs.size(); ++i) {
        bindingForBuffer[attribs[i].type] = i;
    }    
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

}
