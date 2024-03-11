#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

void LoadUFBX(
    const std::string& path,
    const char* objectName, 
    std::vector<float>& vertices,
    std::vector<float>& texcoords, 
    std::vector<float>& normals,
    std::vector<uint8_t>& boneIndices, 
    std::vector<float>& boneWeights,
    std::vector<std::vector<glm::mat4x3>>& animFrames // nested: frame, bone, matrix
);