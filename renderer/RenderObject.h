#pragma once

#include <stdint.h>
#include <gx2r/buffer.h>

struct RenderObjectImpl;
struct RenderMaterial;

enum BufferType {
  VERTEX,
  TEXCOORD,
  NORMAL,
  COLOR,
  PROJECTION,
};

/**
 * A 3D object data object. Vertices, materials, etc.
 * 
 * Also knows how to render itself. Is that poor separation of concerns? You're not my mum.
 */
struct RenderObject {
  RenderObject();
  ~RenderObject();
  virtual void render();
  virtual void setMaterial(RenderMaterial* material);

  // Messing with the buffers involves doing GX2-specific memory locking
  void setProjectionBuffer(const float* data);  // always 16 floats
  void setAttribBuffer(BufferType bt, const float* data, uint32_t elemSize, uint32_t elemCount);
private:
  void setAttribBuffer(const float* data, uint32_t elemSize, uint32_t elemCount, GX2RBuffer* buffer);
  RenderObjectImpl* _impl;
};
