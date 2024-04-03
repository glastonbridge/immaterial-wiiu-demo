
#include "ShaderManager.h"
#include "CRC32.h"
#include <malloc.h>
#include <gx2/mem.h>
#include "../util/wuhbsupport.h"

// #define CACHE_ONLY 1

#ifndef CACHE_ONLY
#include "CafeGLSLCompiler.h"
#endif

// Init GLSL if not inited
bool haveGLSL = false;
void maybeInitGLSL() {
    if (!haveGLSL) {
        WHBLogPrintf("Initializing GLSL compiler");
        #ifndef CACHE_ONLY
        GLSL_Init();
        #endif
        haveGLSL = true;
    }
}

// Build shader cache key
void getShaderCacheFileName(const char* filename, const char* source, char* key) {
    // Part 1: CRC32 of source
    char sourceCrc[9];
    crc32(source, sourceCrc);

    // Part 2: CRC32 of filename
    char filenameCrc[9];
    crc32(filename, filenameCrc);

    // Concat
    sprintf(key, "shader_cache/%s_%s.shader", sourceCrc, filenameCrc);
}

// Check if the shader cache exists and load it if it does
GX2VertexShader* loadVertexShaderFromCache(const char* filename, const char* source) {
    // Build the cache key
    char key[256];
    getShaderCacheFileName(filename, source, key);

    // Load the cache
    WHBLogPrintf("Looking for shader cache %s", key);

    std::string prefixedPath = WIIU_PATH_PREFIX + std::string(key);
    FILE* file = fopen(prefixedPath.c_str(), "rb");
    if (!file) {
        WHBLogPrintf("Shader cache not found");
        return nullptr;
    }

    // Read the shader metadata
    GX2VertexShader* shader = (GX2VertexShader*)memalign(64, sizeof(GX2VertexShader));
    fread(shader, sizeof(GX2VertexShader), 1, file);

    // Do a bit of setup
    shader->gx2rBuffer.flags = GX2R_RESOURCE_BIND_SHADER_PROGRAM |
                            GX2R_RESOURCE_USAGE_CPU_READ |
                            GX2R_RESOURCE_USAGE_CPU_WRITE |
                            GX2R_RESOURCE_USAGE_GPU_READ;
    shader->gx2rBuffer.elemSize = shader->size;
    shader->gx2rBuffer.elemCount = 1;
    shader->gx2rBuffer.buffer = NULL;
    if (!GX2RCreateBuffer(&shader->gx2rBuffer)) {
        WHBLogPrintf("GX2RCreateBuffer failed with programSize = %u", shader->size);
    }
    void* program = GX2RLockBufferEx(&shader->gx2rBuffer, (GX2RResourceFlags)0);
    if (!program) {
        WHBLogPrintf("%s: GX2RLockBufferEx failed", __FUNCTION__);
    }

    // Read the shader program
    shader->program = program;
    fread(program, shader->size, 1, file);

    // Read the uniform blocks
    shader->uniformBlocks = (GX2UniformBlock*)memalign(64, shader->uniformBlockCount * sizeof(GX2UniformBlock));
    fread(shader->uniformBlocks, sizeof(GX2UniformBlock), shader->uniformBlockCount, file);

    // Read the loop vars
    shader->loopVars = (GX2LoopVar*)memalign(64, shader->loopVarCount * sizeof(GX2LoopVar));
    fread(shader->loopVars, sizeof(GX2LoopVar), shader->loopVarCount, file);

    // Read the sampler vars
    shader->samplerVars = (GX2SamplerVar*)memalign(64, shader->samplerVarCount * sizeof(GX2SamplerVar));
    fread(shader->samplerVars, sizeof(GX2SamplerVar), shader->samplerVarCount, file);

    // Read the attrib vars
    shader->attribVars = (GX2AttribVar*)memalign(64, shader->attribVarCount * sizeof(GX2AttribVar));
    fread(shader->attribVars, sizeof(GX2AttribVar), shader->attribVarCount, file);

    // Read the strings
    for(uint32_t i = 0; i < shader->uniformBlockCount; ++i) {
        shader->uniformBlocks[i].name = (char*)memalign(64, 256);
        fread((void*)shader->uniformBlocks[i].name, 256, 1, file);
    }
    for(uint32_t i = 0; i < shader->samplerVarCount; ++i) {
        shader->samplerVars[i].name = (char*)memalign(64, 256);
        fread((void*)shader->samplerVars[i].name, 256, 1, file);
    }
    for(uint32_t i = 0; i < shader->attribVarCount; ++i) {
        shader->attribVars[i].name = (char*)memalign(64, 256);
        fread((void*)shader->attribVars[i].name, 256, 1, file);
    }

    // We're done!
    fclose(file);

    // I stole this from libwhbs GFD shader loader
    GX2RUnlockBufferEx(&shader->gx2rBuffer, GX2R_RESOURCE_DISABLE_CPU_INVALIDATE | GX2R_RESOURCE_DISABLE_GPU_INVALIDATE);
    GX2RUnlockBufferEx(&shader->gx2rBuffer, (GX2RResourceFlags)0);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, shader->program, shader->size);

    return shader;
}

// Check if the shader cache exists and load it if it does
GX2PixelShader* loadPixelShaderFromCache(const char* filename, const char* source) {
    // Build the cache key
    char key[256];
    getShaderCacheFileName(filename, source, key);

    // Load the cache
    WHBLogPrintf("Looking for shader cache %s", key);
    std::string prefixedPath = WIIU_PATH_PREFIX + std::string(key);
    FILE* file = fopen(prefixedPath.c_str(), "rb");
    if (!file) {
        WHBLogPrintf("Shader cache not found");
        return nullptr;
    }

    // Read the shader metadata
    GX2PixelShader* shader = (GX2PixelShader*)memalign(64, sizeof(GX2PixelShader));
    fread(shader, sizeof(GX2PixelShader), 1, file);

    // Do a bit of setup
    shader->gx2rBuffer.flags = GX2R_RESOURCE_BIND_SHADER_PROGRAM |
                            GX2R_RESOURCE_USAGE_CPU_READ |
                            GX2R_RESOURCE_USAGE_CPU_WRITE |
                            GX2R_RESOURCE_USAGE_GPU_READ;
    shader->gx2rBuffer.elemSize = shader->size;
    shader->gx2rBuffer.elemCount = 1;
    shader->gx2rBuffer.buffer = NULL;
    if (!GX2RCreateBuffer(&shader->gx2rBuffer)) {
        WHBLogPrintf("GX2RCreateBuffer failed with programSize = %u", shader->size);
    }
    void* program = GX2RLockBufferEx(&shader->gx2rBuffer, (GX2RResourceFlags)0);
    if (!program) {
        WHBLogPrintf("%s: GX2RLockBufferEx failed", __FUNCTION__);
    }

    // Read the shader program
    shader->program = program;
    fread(program, shader->size, 1, file);

    // Read the uniform blocks
    shader->uniformBlocks = (GX2UniformBlock*)memalign(64, shader->uniformBlockCount * sizeof(GX2UniformBlock));
    fread(shader->uniformBlocks, sizeof(GX2UniformBlock), shader->uniformBlockCount, file);

    // Read the loop vars
    shader->loopVars = (GX2LoopVar*)memalign(64, shader->loopVarCount * sizeof(GX2LoopVar));
    fread(shader->loopVars, sizeof(GX2LoopVar), shader->loopVarCount, file);

    // Read the sampler vars
    shader->samplerVars = (GX2SamplerVar*)memalign(64, shader->samplerVarCount * sizeof(GX2SamplerVar));
    fread(shader->samplerVars, sizeof(GX2SamplerVar), shader->samplerVarCount, file);

    // Read the strings
    for(uint32_t i = 0; i < shader->uniformBlockCount; ++i) {
        shader->uniformBlocks[i].name = (char*)memalign(64, 256);
        fread((void*)shader->uniformBlocks[i].name, 256, 1, file);
    }
    for(uint32_t i = 0; i < shader->samplerVarCount; ++i) {
        shader->samplerVars[i].name = (char*)memalign(64, 256);
        fread((void*)shader->samplerVars[i].name, 256, 1, file);
    }

    // We're done!
    fclose(file);

    // I stole this from libwhbs GFD shader loader
    GX2RUnlockBufferEx(&shader->gx2rBuffer, GX2R_RESOURCE_DISABLE_CPU_INVALIDATE | GX2R_RESOURCE_DISABLE_GPU_INVALIDATE);
    GX2RUnlockBufferEx(&shader->gx2rBuffer, (GX2RResourceFlags)0);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, shader->program, shader->size);

    return shader;
}


// Save the shader cache
bool saveVertexShaderToCache(const char* filename, const char* source, GX2VertexShader* shader) {
    if (!shader || !filename) {
        return false;
    }

    // Build the cache key
    char key[256];
    getShaderCacheFileName(filename, source, key); // Assuming a function to generate the filename from shader's metadata

    WHBLogPrintf("Saving shader cache %s", key);
    std::string prefixedPath = WIIU_PATH_PREFIX + std::string(key);
    FILE* file = fopen(prefixedPath.c_str(), "wb");
    if (!file) {
        WHBLogPrintf("Failed to create shader cache");
        return false;
    }

    // Write the shader metadata
    WHBLogPrintf("Writing shader meta general");
    fwrite(shader, sizeof(GX2VertexShader), 1, file);

    // Write the shader program
    fwrite(shader->program, shader->size, 1, file);

    // Write the uniform blocks
    fwrite(shader->uniformBlocks, sizeof(GX2UniformBlock), shader->uniformBlockCount, file);

    // Write the loop vars
    fwrite(shader->loopVars, sizeof(GX2LoopVar), shader->loopVarCount, file);

    // Write the sampler vars
    fwrite(shader->samplerVars, sizeof(GX2SamplerVar), shader->samplerVarCount, file);

    // Write the attrib vars
    fwrite(shader->attribVars, sizeof(GX2AttribVar), shader->attribVarCount, file);

    // Write the strings
    char nameBuffer[256];
    memset(nameBuffer, 0, sizeof(nameBuffer));
    for (uint32_t i = 0; i < shader->uniformBlockCount; ++i) {
        strncpy(nameBuffer, shader->uniformBlocks[i].name, sizeof(nameBuffer) - 1);
        fwrite(nameBuffer, sizeof(nameBuffer), 1, file);
        memset(nameBuffer, 0, sizeof(nameBuffer));
    }
    for (uint32_t i = 0; i < shader->samplerVarCount; ++i) {
        strncpy(nameBuffer, shader->samplerVars[i].name, sizeof(nameBuffer) - 1);
        fwrite(nameBuffer, sizeof(nameBuffer), 1, file);
        memset(nameBuffer, 0, sizeof(nameBuffer));
    }
    for (uint32_t i = 0; i < shader->attribVarCount; ++i) {
        strncpy(nameBuffer, shader->attribVars[i].name, sizeof(nameBuffer) - 1);
        fwrite(nameBuffer, sizeof(nameBuffer), 1, file);
        memset(nameBuffer, 0, sizeof(nameBuffer));
    }

    fclose(file);
    WHBLogPrintf("Shader cache saved successfully");
    return true;
}
bool savePixelShaderToCache(const char* filename, const char* source, GX2PixelShader* shader) {
    if (!shader || !filename) {
        return false;
    }

    // Build the cache key
    char key[256];
    getShaderCacheFileName(filename, source, key); // Assuming a function to generate the filename from shader's metadata

    WHBLogPrintf("Saving shader cache %s", key);
    std::string prefixedPath = WIIU_PATH_PREFIX + std::string(key);
    FILE* file = fopen(prefixedPath.c_str(), "wb");
    if (!file) {
        WHBLogPrintf("Failed to create shader cache");
        return false;
    }

    // Write the shader metadata
    WHBLogPrintf("Writing shader meta general");
    fwrite(shader, sizeof(GX2PixelShader), 1, file);

    // Write the shader program
    fwrite(shader->program, shader->size, 1, file);

    // Write the uniform blocks
    fwrite(shader->uniformBlocks, sizeof(GX2UniformBlock), shader->uniformBlockCount, file);

    // Write the loop vars
    fwrite(shader->loopVars, sizeof(GX2LoopVar), shader->loopVarCount, file);

    // Write the sampler vars
    fwrite(shader->samplerVars, sizeof(GX2SamplerVar), shader->samplerVarCount, file);

    // Write the strings
    char nameBuffer[256];
    memset(nameBuffer, 0, sizeof(nameBuffer));
    for (uint32_t i = 0; i < shader->uniformBlockCount; ++i) {
        strncpy(nameBuffer, shader->uniformBlocks[i].name, sizeof(nameBuffer) - 1);
        fwrite(nameBuffer, sizeof(nameBuffer), 1, file);
        memset(nameBuffer, 0, sizeof(nameBuffer));
    }
    for (uint32_t i = 0; i < shader->samplerVarCount; ++i) {
        strncpy(nameBuffer, shader->samplerVars[i].name, sizeof(nameBuffer) - 1);
        fwrite(nameBuffer, sizeof(nameBuffer), 1, file);
        memset(nameBuffer, 0, sizeof(nameBuffer));
    }

    fclose(file);
    WHBLogPrintf("Shader cache saved successfully");
    return true;
}

// Compiler/cache loader
WHBGfxShaderGroup* compileOrLoadShader(const char* vsPath, const char* psPath, const char *vsSrc, const char *psSrc)
{
    char infoLog[1024];
    GX2VertexShader *vs = loadVertexShaderFromCache(vsPath, vsSrc);
    if(!vs) {
        WHBLogPrintf("Cache load failed, compiling vertex shader");
#ifndef CACHE_ONLY        
        maybeInitGLSL();
        vs = GLSL_CompileVertexShader(vsSrc, infoLog, sizeof(infoLog), GLSL_COMPILER_FLAG_NONE);
#endif        
        if(!vs) {
            WHBLogPrintf("Failed to either load or compile vertex shader. Infolog: %s\n", infoLog);
            return NULL;
        }
        saveVertexShaderToCache(vsPath, vsSrc, vs);
    }
    else {
        WHBLogPrintf("Cache load successful, using cached vertex shader.");
    }
    //WHBLogPrintf("Compiled vertex shader. Buffers that are set currently are: uniformBlocks %p, uniformVars %p, initialValues %p, loopVars %p, samplerVars %p, attribVars %p", vs->uniformBlocks, vs->uniformVars, vs->initialValues, vs->loopVars, vs->samplerVars, vs->attribVars);
        
    GX2PixelShader *ps = loadPixelShaderFromCache(psPath, psSrc);
    if(!ps) {
        WHBLogPrintf("Cache load failed, compiling pixel shader");
#ifndef CACHE_ONLY        
        maybeInitGLSL();
        ps = GLSL_CompilePixelShader(psSrc, infoLog, sizeof(infoLog), GLSL_COMPILER_FLAG_NONE);
#endif        
        if(!ps) {
            WHBLogPrintf("Failed to compile pixel shader. Infolog: %s\n", infoLog);
            return NULL;
        }
        savePixelShaderToCache(psPath, psSrc, ps);
    }
    else {
        WHBLogPrintf("Cache load successful, using cached pixel shader.");
    }
    //WHBLogPrintf("Compiled pixel shader. Buffers that are set currently are: uniformBlocks %p, uniformVars %p, initialValues %p, loopVars %p, samplerVars %p", ps->uniformBlocks, ps->uniformVars, ps->initialValues, ps->loopVars, ps->samplerVars);
    
    WHBGfxShaderGroup *shaderGroup = (WHBGfxShaderGroup *)malloc(sizeof(WHBGfxShaderGroup));
    memset(shaderGroup, 0, sizeof(*shaderGroup));
    shaderGroup->vertexShader = vs;
    shaderGroup->pixelShader = ps;   
    return shaderGroup;
}

char* loadShader(const char* filename) {
    // just use the C file operations for now
    std::string prefixedPath = WIIU_PATH_PREFIX + std::string(filename);
    WHBLogPrintf("Loading shader code from %s", prefixedPath.c_str());
    FILE* file = fopen(prefixedPath.c_str(), "rb");
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
    attribs.push_back(AttribSpec {"in_texcoord", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
    shader("shaders/billboard.vert", "shaders/blur.frag", attribs);
    attribs.clear();

    attribs.push_back(AttribSpec {"in_position", BufferType::VERTEX, GX2_ATTRIB_FORMAT_FLOAT_32_32_32});
    attribs.push_back(AttribSpec {"in_texcoord", BufferType::COLOR, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32});
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
        // TODO: free the shaders without crashing
        //GLSL_FreeVertexShader(group->vertexShader);
        //GLSL_FreePixelShader(group->pixelShader);
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
    WHBGfxShaderGroup* group = compileOrLoadShader(vsPath, psPath, vsCode, psCode);
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
