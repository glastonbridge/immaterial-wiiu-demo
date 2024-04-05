
#include "../graphics/SceneObject.h"
#include "RenderBuffer.h"
#include "RenderTexture.h"

struct SceneBase;
struct Renderer {
public:
  Renderer();
  ~Renderer();
  void renderFrame(const SceneBase &scene);

private:
  std::unique_ptr<RenderObject> composeQuad;
  std::unique_ptr<RenderObject> blurQuad;
  std::unique_ptr<RenderBuffer> bufferA;
  std::unique_ptr<RenderBuffer> bufferB;
};
