#pragma once

#include <cstdint>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <gx2r/buffer.h>
#include <memory>
#include <vector>

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

enum UniformType { CAMERA_PROJECTION, TRANSFORM, BONE_TRANSFORM, CAMERA_VIEW };

/**
 * Data related to a specific instance of a render object.
 * This is used for variable uniforms such as object transform.
 */
struct RenderInstance {
  RenderInstance();
  ~RenderInstance();

  void setUniformFloatMat(UniformType bt, const float *mat,
                                  size_t numFloats);

  GX2RBuffer transformBuffer = {};
  GX2RBuffer boneTransformBuffer = {};
};

/**
 * The parts of an object that are concerned with the rendering implementation
 * This contains the mesh data, material etc. If we don't expect it to be
 * modified after creation, it goes here. A single render object can be rendered
 * multiple times in different places in a scene.
 */
struct RenderObject {
  virtual ~RenderObject() = default;
  virtual void render(RenderInstance const &ri) = 0;
  virtual void setMaterial(RenderMaterial *material) = 0;
  virtual RenderMaterial *getMaterial() = 0;

  // Messing with the buffers involves doing GX2-specific memory locking
  virtual void setAttribBuffer(BufferType bt, const void *data,
                               uint32_t elemSize, size_t elemCount) = 0;
  virtual void setUniformFloatMat(UniformType bt, const float *mat,
                                  size_t numFloats) = 0;
  virtual void setExtraUniform(int index, glm::vec4 data) = 0;

  std::vector<std::vector<glm::mat4>> animFrames;
  void getAnimFrame(float frame, float *boneBuffer) const;

  static std::unique_ptr<RenderObject> create();
  void load(const char *path, const char *name, RenderMaterial *material);

 protected:
  RenderObject() = default;
};
