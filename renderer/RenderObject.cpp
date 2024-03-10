#include "RenderObject.h"
#include "RenderMaterial.h"
#include "../util/memory.h"

#include <dmae/mem.h>

#include <gx2/mem.h>
#include <gx2/shaders.h>
#include <gx2r/buffer.h>
#include <gx2/draw.h>
#include <gx2r/draw.h>

#include <whb/gfx.h>
#include <whb/log.h>


struct RenderObjectImpl {
  RenderMaterial* material;

  // TODO: long overdue some refactoring...
  GX2RBuffer positionBuffer = {};
  GX2RBuffer colourBuffer = {};
  GX2RBuffer texcoordBuffer = {};
  GX2RBuffer normalBuffer = {};
  GX2RBuffer projectionBuffer = {};
  GX2RBuffer transformBuffer = {};

  RenderObjectImpl() {
    projectionBuffer.flags = GX2R_RESOURCE_BIND_UNIFORM_BLOCK |
                             GX2R_RESOURCE_USAGE_CPU_READ |
                             GX2R_RESOURCE_USAGE_CPU_WRITE |
                             GX2R_RESOURCE_USAGE_GPU_READ;
    projectionBuffer.elemSize = 4 * 4 * 4;
    projectionBuffer.elemCount = 1;
    GX2RCreateBuffer(&projectionBuffer);
    transformBuffer.flags = GX2R_RESOURCE_BIND_UNIFORM_BLOCK |
                             GX2R_RESOURCE_USAGE_CPU_READ |
                             GX2R_RESOURCE_USAGE_CPU_WRITE |
                             GX2R_RESOURCE_USAGE_GPU_READ;
    transformBuffer.elemSize = 4 * 4 * 4;
    transformBuffer.elemCount = 1;
    GX2RCreateBuffer(&transformBuffer);
  }

  void setAttribBuffer(BufferType bt, const float* data, uint32_t elemSize, size_t elemCount) {
    GX2RBuffer* buffer;
    if (BufferType::VERTEX == bt) {
      buffer = &positionBuffer;
    } else if (BufferType::COLOR == bt) {
      buffer = &colourBuffer;
    } else if (BufferType::TEXCOORD == bt) {
      buffer = &texcoordBuffer;
    } else if (BufferType::NORMAL == bt) {
      buffer = &normalBuffer;
    }
    setAttribBuffer(data, elemSize, elemCount, buffer);
  }

  void setAttribBuffer(const float* data, uint32_t elemSize, size_t elemCount, GX2RBuffer* buffer) {
    //if(!GX2RBufferExists(buffer)) {
      buffer->flags = static_cast<GX2RResourceFlags>(
        GX2R_RESOURCE_BIND_VERTEX_BUFFER |
        GX2R_RESOURCE_USAGE_CPU_READ |
        GX2R_RESOURCE_USAGE_CPU_WRITE |
        GX2R_RESOURCE_USAGE_GPU_READ);
      buffer->elemSize = elemSize;
      buffer->elemCount = elemCount;
      GX2RCreateBuffer(buffer);
    //}
    void* bufferData = GX2RLockBufferEx(buffer, GX2R_RESOURCE_BIND_NONE);
    memcpy(bufferData, data, buffer->elemSize * buffer->elemCount);
    GX2RUnlockBufferEx(buffer, GX2R_RESOURCE_BIND_NONE);
  }

  void setUniformFloatMat(UniformType bt, const float* mat, size_t numFloats) {
    GX2RBuffer* buffer;
    if (UniformType::CAMERA_PROJECTION==bt) {
      buffer = &projectionBuffer;
    } else if (UniformType::TRANSFORM==bt) {
      buffer = &transformBuffer;
    }
    setUniformFloatMat(mat, numFloats, buffer);
  }

  void setUniformFloatMat(const float* matPtr, size_t numFloats, GX2RBuffer* buffer) {
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, (void *)buffer, numFloats * 4);
    void* bufferData = GX2RLockBufferEx(buffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, (void *)buffer, numFloats * 4);
    swap_memcpy(bufferData, matPtr, numFloats * 4);
    GX2RUnlockBufferEx(buffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  }

  void render() {
    void *buffer = NULL;

    material->renderUsing();

    GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&texcoordBuffer, 2, texcoordBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&normalBuffer, 3, normalBuffer.elemSize, 0);
    GX2RSetVertexUniformBlock(&projectionBuffer, 0, 0);
    GX2RSetVertexUniformBlock(&transformBuffer, 1, 0);

    GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, positionBuffer.elemCount, 0, 1);

  }
  ~RenderObjectImpl() {

    WHBLogPrintf("Destroying RenderObject"); 
    GX2RDestroyBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&texcoordBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&normalBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&transformBuffer, GX2R_RESOURCE_BIND_NONE);
  }
};

RenderObject::RenderObject() {
  _impl = new RenderObjectImpl();
}

RenderObject::~RenderObject() {
  delete _impl;
}


void RenderObject::render() { _impl -> render(); }
void RenderObject::setAttribBuffer(BufferType bt, const float* data, uint32_t elemSize, uint32_t elemCount)  { 
  _impl->setAttribBuffer(bt, data, elemSize, elemCount);
}
void RenderObject::setUniformFloatMat(UniformType bt, const float* mat, size_t numFloats) {
  _impl->setUniformFloatMat(bt, mat, numFloats);
}
void RenderObject::setMaterial(RenderMaterial* material) {_impl->material = material;}