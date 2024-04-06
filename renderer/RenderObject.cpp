#include "RenderObject.h"
#include "../util/memory.h"
#include "RenderMaterial.h"

#include <dmae/mem.h>

#include <gx2/draw.h>
#include <gx2/mem.h>
#include <gx2/shaders.h>
#include <gx2r/draw.h>

#include <whb/gfx.h>
#include <whb/log.h>

struct RenderObjectImpl : RenderObject {
  RenderMaterial *material;

  GX2RBuffer positionBuffer = {};
  GX2RBuffer colourBuffer = {};
  GX2RBuffer texcoordBuffer = {};
  GX2RBuffer normalBuffer = {};
  GX2RBuffer boneIdxBuffer = {};
  GX2RBuffer boneWeightBuffer = {};

  RenderObjectImpl() {
  }

  void setAttribBuffer(BufferType bt, const void *data, uint32_t elemSize,
                       size_t elemCount) final {
    GX2RBuffer *buffer;
    if (BufferType::VERTEX == bt) {
      buffer = &positionBuffer;
    } else if (BufferType::COLOR == bt) {
      buffer = &colourBuffer;
    } else if (BufferType::TEXCOORD == bt) {
      buffer = &texcoordBuffer;
    } else if (BufferType::NORMAL == bt) {
      buffer = &normalBuffer;
    } else if (BufferType::BONE_IDX == bt) {
      buffer = &boneIdxBuffer;
    } else if (BufferType::BONE_WEIGHT == bt) {
      buffer = &boneWeightBuffer;
    } else {
      WHBLogPrintf("Not an attribute buffer type");
      return;
    }

    if (!GX2RBufferExists(buffer)) {
      buffer->flags = static_cast<GX2RResourceFlags>(
          GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ |
          GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ);
      buffer->elemSize = elemSize;
      buffer->elemCount = elemCount;
      GX2RCreateBuffer(buffer);
    }
    void *bufferData = GX2RLockBufferEx(buffer, GX2R_RESOURCE_BIND_NONE);
    memcpy(bufferData, data, buffer->elemSize * buffer->elemCount);
    GX2RUnlockBufferEx(buffer, GX2R_RESOURCE_BIND_NONE);
  }

  void render(RenderInstance const &instance, RenderView const &view) const final {
    material->renderUsing();

    if (material->getBindingForBuffer(BufferType::VERTEX) != -1) {
      GX2RSetAttributeBuffer(const_cast<GX2RBuffer *>(&positionBuffer),
                             material->getBindingForBuffer(BufferType::VERTEX),
                             positionBuffer.elemSize, 0);
    }
    if (material->getBindingForBuffer(BufferType::COLOR) != -1) {
      GX2RSetAttributeBuffer(const_cast<GX2RBuffer *>(&colourBuffer),
                             material->getBindingForBuffer(BufferType::COLOR),
                             colourBuffer.elemSize, 0);
    }
    if (material->getBindingForBuffer(BufferType::TEXCOORD) != -1) {
      GX2RSetAttributeBuffer(
          const_cast<GX2RBuffer *>(&texcoordBuffer),
          material->getBindingForBuffer(BufferType::TEXCOORD),
          texcoordBuffer.elemSize, 0);
    }
    if (material->getBindingForBuffer(BufferType::NORMAL) != -1) {
      GX2RSetAttributeBuffer(const_cast<GX2RBuffer *>(&normalBuffer),
                             material->getBindingForBuffer(BufferType::NORMAL),
                             normalBuffer.elemSize, 0);
    }
    if (material->getBindingForBuffer(BufferType::BONE_IDX) != -1) {
      GX2RSetAttributeBuffer(
          const_cast<GX2RBuffer *>(&boneIdxBuffer),
          material->getBindingForBuffer(BufferType::BONE_IDX),
          boneIdxBuffer.elemSize, 0);
    }
    if (material->getBindingForBuffer(BufferType::BONE_WEIGHT) != -1) {
      GX2RSetAttributeBuffer(
          const_cast<GX2RBuffer *>(&boneWeightBuffer),
          material->getBindingForBuffer(BufferType::BONE_WEIGHT),
          boneWeightBuffer.elemSize, 0);
    }

    // The const_casts here are for the benefit of C..
    // We don't expect the buffers to get modified at all.
    GX2RSetVertexUniformBlock(const_cast<GX2RBuffer *>(&view.projectionBuffer), 0, 0);
    GX2RSetVertexUniformBlock(const_cast<GX2RBuffer *>(&instance.transformBuffer), 1, 0);
    GX2RSetVertexUniformBlock(const_cast<GX2RBuffer *>(&instance.boneTransformBuffer), 2, 0);
    GX2RSetVertexUniformBlock(const_cast<GX2RBuffer *>(&view.viewBuffer), 3, 0);
    GX2RSetVertexUniformBlock(const_cast<GX2RBuffer *>(&view.extraBuffer), 4, 0);
    GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, positionBuffer.elemCount, 0, 1);
  }

  void setMaterial(RenderMaterial *p_material) final { material = p_material; }
  RenderMaterial *getMaterial() const final { return material; }

  ~RenderObjectImpl() {
    WHBLogPrintf("Destroying RenderObject");
    GX2RDestroyBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&texcoordBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&normalBuffer, GX2R_RESOURCE_BIND_NONE);
  }
};

std::unique_ptr<RenderObject> RenderObject::create() {
  return std::make_unique<RenderObjectImpl>();
}

void RenderObject::applyAnimation(float frame, RenderInstance &instance) const {
  // Bail if we don't have any frames
  if (animFrames.empty()) {
    glm::mat4 boneFrameMat = glm::mat4(1.0f);
    instance.setUniformFloatMat(UniformType::BONE_TRANSFORM,
                       glm::value_ptr(boneFrameMat), 4 * 4);
    return;
  }

  // Alloc buffer if we need one
  size_t numBones = animFrames[0].size();
  if (instance.numBones != numBones) {
    WHBLogPrintf("Allocating bone interp buffer for %d bones", numBones);
    instance.boneMatInterpBuffer.reset(new float[4 * 4 * numBones]);
    instance.numBones = numBones;
  }

  // Figure out where in the animation we are
  size_t numFrames = animFrames.size();
  int animPos = (int)frame;
  float animPosRemainder = frame - (float)animPos;
  animPos = animPos % numFrames;
  int animPosNext = (animPos + 1) % numFrames;
  WHBLogPrintf("animPos: %d, animPosNext: %d, animPosRemainder: %f", animPos,
               animPosNext, animPosRemainder);

  // Copy all the bone mats into the array, interpolating linearly between two
  // adjacent frames
  for (int i = 0; i < numBones; i++) {
    glm::mat4 boneFrameMat =
        animFrames[animPos][i] * (1.0f - animPosRemainder) +
        animFrames[animPosNext][i] * animPosRemainder;
    memcpy(instance.boneMatInterpBuffer.get() + (i * 4 * 4), glm::value_ptr(boneFrameMat),
           4 * 4 * sizeof(float));
  }

  // Bones to shader buffer
  WHBLogPrintf("Setting bone transform uniform");
  instance.setUniformFloatMat(UniformType::BONE_TRANSFORM,
                     instance.boneMatInterpBuffer.get(), 4 * 4 * numBones);
}

RenderInstance::RenderInstance() {
  transformBuffer.flags =
      GX2R_RESOURCE_BIND_UNIFORM_BLOCK | GX2R_RESOURCE_USAGE_CPU_READ |
      GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
  transformBuffer.elemSize = 4 * 4 * 4;
  transformBuffer.elemCount = 1;
  GX2RCreateBuffer(&transformBuffer);

  boneTransformBuffer.flags =
      GX2R_RESOURCE_BIND_UNIFORM_BLOCK | GX2R_RESOURCE_USAGE_CPU_READ |
      GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
  boneTransformBuffer.elemSize = 4 * 4 * 4;
  boneTransformBuffer.elemCount = 32; // up to 32 bones
  GX2RCreateBuffer(&boneTransformBuffer);
}

RenderInstance::~RenderInstance() {
  WHBLogPrintf("Destroying RenderInstance");
  GX2RDestroyBufferEx(&transformBuffer, GX2R_RESOURCE_BIND_NONE);
  GX2RDestroyBufferEx(&boneTransformBuffer, GX2R_RESOURCE_BIND_NONE);
}

void RenderInstance::setUniformFloatMat(UniformType bt, const float *mat,
                          size_t numFloats) {
  GX2RBuffer *buffer;
  if (UniformType::TRANSFORM == bt) {
    buffer = &transformBuffer;
  } else if (UniformType::BONE_TRANSFORM == bt) {
    buffer = &boneTransformBuffer;
  } else {
    WHBLogPrintf("Not a valid matrix uniform type for RenderInstance");
    return;
  }

  GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK,
                (void *)buffer, numFloats * 4);
  void *bufferData =
      GX2RLockBufferEx(buffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK,
                (void *)buffer, numFloats * 4);
  swap_memcpy(bufferData, mat, numFloats * 4);
  GX2RUnlockBufferEx(buffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
}

RenderView::RenderView() {
  projectionBuffer.flags =
      GX2R_RESOURCE_BIND_UNIFORM_BLOCK | GX2R_RESOURCE_USAGE_CPU_READ |
      GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
  projectionBuffer.elemSize = 4 * 4 * 4;
  projectionBuffer.elemCount = 1;
  GX2RCreateBuffer(&projectionBuffer);

  viewBuffer.flags =
      GX2R_RESOURCE_BIND_UNIFORM_BLOCK | GX2R_RESOURCE_USAGE_CPU_READ |
      GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
  viewBuffer.elemSize = 4 * 4 * 4;
  viewBuffer.elemCount = 1;
  GX2RCreateBuffer(&viewBuffer);

  extraBuffer.flags =
      GX2R_RESOURCE_BIND_UNIFORM_BLOCK | GX2R_RESOURCE_USAGE_CPU_READ |
      GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
  extraBuffer.elemSize = 4 * 4; // extra uniforms are vec4
  extraBuffer.elemCount = 1;
  GX2RCreateBuffer(&extraBuffer);
}

RenderView::~RenderView() {
  WHBLogPrintf("Destroying RenderView");
  GX2RDestroyBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_NONE);
  GX2RDestroyBufferEx(&viewBuffer, GX2R_RESOURCE_BIND_NONE);
  GX2RDestroyBufferEx(&extraBuffer, GX2R_RESOURCE_BIND_NONE);
}

void RenderView::setUniformFloatMat(UniformType bt, const float *mat,
                          size_t numFloats) {
  GX2RBuffer *buffer;
  if (UniformType::CAMERA_PROJECTION == bt) {
    buffer = &projectionBuffer;
  } else if (UniformType::CAMERA_VIEW == bt) {
    buffer = &viewBuffer;
  } else {
    WHBLogPrintf("Not a valid matrix uniform type for RenderInstance");
    return;
  }

  GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK,
                (void *)buffer, numFloats * 4);
  void *bufferData =
      GX2RLockBufferEx(buffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK,
                (void *)buffer, numFloats * 4);
  swap_memcpy(bufferData, mat, numFloats * 4);
  GX2RUnlockBufferEx(buffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
}

void RenderView::setExtraUniform(int index, glm::vec4 data) {
  if (index >= 4) {
    WHBLogPrintf("Extra uniform index out of range");
    return;
  }
  GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK,
                (void *)&extraBuffer, 4 * 4);
  float *bufferData = (float *)GX2RLockBufferEx(
      &extraBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK,
                (void *)&extraBuffer, 4 * 4);
  swap_memcpy(bufferData, glm::value_ptr(data), 4 * 4);
  GX2RUnlockBufferEx(&extraBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
}

