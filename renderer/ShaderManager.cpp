
#include "ShaderManager.h"

// Helpers
WHBGfxShaderGroup* GLSL_CompileShader(const char *vsSrc, const char *psSrc)
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

char* loadShader(const char* filename) {
  // TODO: these WHB file operations are deprecated and should have been replaced with standard C++
  // file operations. But using the C++ file operations somehow causes our app to not quit properly
  // and the console crashes when trying to quit to menu. Why??

  char *sdRootPath = WHBGetSdCardMountPath();
  char path[256];
  sprintf(path, "%s/%s", sdRootPath, filename);
  WHBLogPrintf("Loading shader %s", path);

   // just use the C file operations for now
   FILE* file = fopen(path, "rb");
   fseek(file, 0, SEEK_END);
   size_t size = ftell(file);
   fseek(file, 0, SEEK_SET);
   char* text = (char*)malloc(size + 1);
   fread(text, 1, size, file);
   text[size] = 0;
   fclose(file);
   return text;
}

static ShaderManager* globalShaderManager = nullptr;
ShaderManager* getShaderManager() {
    if (globalShaderManager == nullptr) {
        globalShaderManager = new ShaderManager();
    }
    return globalShaderManager;
}

ShaderManager::ShaderManager() {
    // Preheat the shader cache
    std::vector<AttribSpec> attribs;
    
    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    shader("shaders/billboard.vert", "shaders/blur.frag", attribs);
    attribs.clear();

    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    shader("shaders/billboard.vert", "shaders/compose.frag", attribs);
    attribs.clear();

    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_texcoord", BufferType::TEXCOORD, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    attribs.push_back(AttribSpec {"in_normal", BufferType::NORMAL, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_bone_idx", BufferType::BONE_IDX, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    attribs.push_back(AttribSpec {"in_bone_weight", BufferType::BONE_WEIGHT, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    shader("shaders/bones.vert", "shaders/bones.frag", attribs);
    attribs.clear();

    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    shader("shaders/projected.vert", "shaders/projected.frag", attribs);
    attribs.clear();
 
    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_color", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    attribs.push_back(AttribSpec {"in_texcoord", BufferType::TEXCOORD, GX2_ATTRIB_FORMAT_FLOAT_32_32});
    attribs.push_back(AttribSpec {"in_normal", BufferType::NORMAL, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    shader("shaders/projected.vert", "shaders/textured.frag", attribs);
    attribs.clear();

}

ShaderManager::~ShaderManager() {
    // Free all the shaders
    for (auto it = shaders.begin(); it != shaders.end(); ++it) {
        WHBGfxShaderGroup* group = it->second;
        GLSL_FreeVertexShader(group->vertexShader);
        GLSL_FreePixelShader(group->pixelShader);
        free(group);
    }
}

WHBGfxShaderGroup* ShaderManager::shader(const char* vsPath, const char* psPath, const std::vector<AttribSpec>& attribs) {
    // Concat the names as key 
    std::string key = std::string(vsPath) + "|" + std::string(psPath);
    
    // Also add the attribs to the key
    for (uint32_t i = 0; i < attribs.size(); ++i) {
        key += "|" + std::string(attribs[i].name);
    }

    WHBLogPrintf("Looking for shader %s", key.c_str());
    if (shaders.find(key) != shaders.end()) {
        WHBLogPrintf("Found shader %s = %p", key.c_str(), shaders[key]);
        return shaders[key];
    }

    // Load the shaders
    char* vsCode = loadShader(vsPath);
    char* psCode = loadShader(psPath);

    // Compile the shaders
    WHBLogPrintf("Compiling %s %s", vsPath, psPath);
    WHBGfxShaderGroup* group = GLSL_CompileShader(vsCode, psCode);
    if (!group) {
        WHBLogPrintf("Shader compilation failed");
    } else {
        WHBLogPrintf("Shader compilation completed");
    }

    for (uint32_t i = 0; i < attribs.size(); ++i) {
        WHBLogPrintf("Attaching attributes %s - %i @ %d", attribs[i].name.c_str(), attribs[i].format, i);
        WHBGfxInitShaderAttribute(group, attribs[i].name.c_str(), i, 0, attribs[i].format);
    }
    WHBGfxInitFetchShader(group);

    // Store the shaders
    shaders[key] = group;
    free(vsCode);
    free(psCode);
    WHBLogPrintf("Stored shader %s = %p", key.c_str(), group);
    return group;
}
