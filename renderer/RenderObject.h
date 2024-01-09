#pragma once

#include <stdint.h>
#include <gx2r/buffer.h>

struct RenderObjectImpl;
struct Material;

/**
 * A 3D object data object. Vertices, materials, etc.
 * 
 * Also knows how to render itself. Is that poor separation of concerns? You're not my mum.
 */
struct RenderObject {
  RenderObject();
  ~RenderObject();
  virtual void render();
  virtual void setMaterial(Material* material);

//protected:
  // Messing with the buffers involves doing GX2-specific memory locking
  void setPositionBuffer(const float* data, uint32_t elemSize, uint32_t elemCount);
  void setColourBuffer(const float* data, uint32_t elemSize, uint32_t elemCount);
  void setTexcoordBuffer(const float* data, uint32_t elemSize, uint32_t elemCount);
  void setProjectionBuffer(const float* data);  // always 16 floats

private:
  void setAttribBuffer(const float* data, uint32_t elemSize, uint32_t elemCount, GX2RBuffer* buffer);
  RenderObjectImpl* _impl;
};
