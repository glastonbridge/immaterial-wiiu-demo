
#include "RenderTexture.h"
#include "RenderBuffer.h"
#include "../graphics/SceneObject.h"

struct SceneBase;
struct Renderer {
  public:
    Renderer();
    ~Renderer();
    void renderFrame(const SceneBase& scene);

  private:
    std::unique_ptr<SceneObject> composeQuad;
    std::unique_ptr<SceneObject> blurQuad;
    std::unique_ptr<RenderBuffer> bufferA;
    std::unique_ptr<RenderBuffer> bufferB;
};
