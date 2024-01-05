struct WHBGfxShaderGroup;

struct Material {
  void attachBillboardShaders(); // initialise the shaders for this material as billboard
  void attachPerspectiveShaders(); // initialise the shaders for this material as perspective
  
  void renderUsing() const; // Attach to gx2 during a render phase

  private: 
  WHBGfxShaderGroup * group;
};
