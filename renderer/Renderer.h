#pragma once

#include <vector>
#include <memory>

struct SceneBase;
struct RenderObject;
struct RenderMaterial;
struct RenderBuffer;
struct RenderInstance;

struct Renderer {
public:
  Renderer();
  ~Renderer();
  void renderFrame(const SceneBase &scene);

private:
  std::unique_ptr<RenderObject> composeQuad;
  std::unique_ptr<RenderObject> blurQuad;
  std::unique_ptr<RenderMaterial> composeMaterial;
  std::unique_ptr<RenderMaterial> blurMaterial;

  std::unique_ptr<RenderBuffer> bufferA;
  std::unique_ptr<RenderBuffer> bufferB;

  std::vector<RenderInstance> instances;
  std::unique_ptr<RenderInstance> quadInstance;
};
