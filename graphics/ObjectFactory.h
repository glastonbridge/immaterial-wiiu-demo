#pragma once
#include <vector>
#include <memory>

struct RenderMaterial;
struct RenderObject;

struct ObjectFactory {
  static std::unique_ptr<RenderObject> load(const char *path, const char *name,
                        RenderMaterial *material);
  static std::unique_ptr<RenderObject> createQuad(RenderMaterial *material);
  static std::unique_ptr<RenderObject> createText(RenderMaterial *material, std::string text,
                                      std::vector<std::vector<float>> vertices,
                                      std::vector<std::vector<float>> normals);
};