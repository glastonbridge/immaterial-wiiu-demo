#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct RenderMaterial;

struct Model {
  std::vector<float> vertices;
  std::vector<float> vertexColours;
  std::vector<float> texCoords;
  std::vector<float> normals;
  std::vector<float> boneIndices;
  std::vector<float> boneWeights;
  std::vector<std::vector<glm::mat4>> animFrames;

  RenderMaterial *material;
};

struct ModelFactory {
  static Model load(const char *path, const char *name,
                    RenderMaterial *material);
  static Model
  createText(RenderMaterial *material, std::string text,
             std::vector<std::vector<float>> const &vertices,
             std::vector<std::vector<float>> const &normals);
};