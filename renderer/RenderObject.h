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
  BONE_IDX,
  BONE_WEIGHT,
  _MAX
};

enum UniformType {
  CAMERA_PROJECTION,
  TRANSFORM,
  BONE_TRANSFORM
};

/**
 * The parts of an object that are concerned with the rendering implementation
 */
struct RenderObject {
  RenderObject();
  ~RenderObject();
  virtual void render();
  virtual void setMaterial(RenderMaterial* material);
  virtual RenderMaterial* getMaterial();

  // Messing with the buffers involves doing GX2-specific memory locking
  void setAttribBuffer(BufferType bt, const void* data, uint32_t elemSize, size_t elemCount);
  void setUniformFloatMat(UniformType bt, const float* mat, size_t numFloats);
  RenderObjectImpl* _impl;
};
