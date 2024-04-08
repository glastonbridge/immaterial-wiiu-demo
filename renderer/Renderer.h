#pragma once

#include <memory>
#include <vector>

struct SceneBase;
struct RenderObject;
struct RenderMaterial;
struct RenderBuffer;
struct RenderInstance;
struct RenderView;
struct Model;

struct Renderer {
public:
  Renderer();
  ~Renderer();
  void renderFrame(SceneBase const &scene, RenderBuffer &rb);
  void addModel(Model const &model);
  void addModel(Model &&model);

  // Pre-allocate buffers for n objects
  void reserve(size_t n);

private:
  std::vector<std::unique_ptr<RenderObject>> objectList;
  std::unique_ptr<RenderObject> composeQuad;
  std::unique_ptr<RenderObject> blurQuad;
  std::unique_ptr<RenderMaterial> composeMaterial;
  std::unique_ptr<RenderMaterial> blurMaterial;

  std::unique_ptr<RenderBuffer> bufferA;
  std::unique_ptr<RenderBuffer> bufferB;

  std::vector<RenderInstance> instances;
  std::unique_ptr<RenderInstance> quadInstance;
  std::unique_ptr<RenderView> view;
  std::unique_ptr<RenderView> postView;
};
