
#include "RenderTexture.h"

struct SceneBase;
struct Renderer {
  Renderer();
  ~Renderer();
  void renderFrame(const SceneBase& scene);

  // TEMP HACK
  BufferTexture* renderBuffer;
};
