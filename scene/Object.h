struct ObjectImpl;

/**
 * A 3D object data object. Vertices, materials, etc.
 * 
 * Also knows how to render itself. Is that poor separation of concerns? You're not my mum.
 */
struct Object {
  virtual void render() =0;
};

Object* LoadObject(const char* path);
