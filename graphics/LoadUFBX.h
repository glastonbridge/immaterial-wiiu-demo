#pragma once

#include <string>
#include <vector>

void LoadUFBX(
    const std::string& path,
    const char* objectName, 
    std::vector<float>& vertices,
    std::vector<float>& texcoords, 
    std::vector<float>& normals,
    std::vector<int>& boneIndices, 
    std::vector<float>& boneWeights,
    std::vector<float>& animFrames // TODO maybe we could sample live here instead of storing at target rate? also, return these as mat3x4s? in a nice nested array?
);