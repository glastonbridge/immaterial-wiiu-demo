struct WHBGfxShaderGroup;

struct Material {
  void attachShaders(); // initialise the shaders for this material
  void renderUsing() const; // Attach to gx2 during a render phase

  private: 
  WHBGfxShaderGroup * group;
};
