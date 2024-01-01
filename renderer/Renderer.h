struct SceneBase;
struct Renderer {
  Renderer();
  ~Renderer();
  void renderFrame(const SceneBase& scene);
};
