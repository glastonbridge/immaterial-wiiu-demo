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
  GX2RBuffer boneIdxBuffer = {};
  GX2RBuffer boneWeightBuffer = {};  
  GX2RBuffer projectionBuffer = {};
  GX2RBuffer transformBuffer = {};
  GX2RBuffer viewBuffer = {};
  GX2RBuffer boneTransformBuffer = {};
  GX2RBuffer extraBuffer = {};

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

    viewBuffer.flags = GX2R_RESOURCE_BIND_UNIFORM_BLOCK |
                             GX2R_RESOURCE_USAGE_CPU_READ |
                             GX2R_RESOURCE_USAGE_CPU_WRITE |
                             GX2R_RESOURCE_USAGE_GPU_READ;
    viewBuffer.elemSize = 4 * 4 * 4;
    viewBuffer.elemCount = 1;                             
    GX2RCreateBuffer(&viewBuffer);

    boneTransformBuffer.flags = GX2R_RESOURCE_BIND_UNIFORM_BLOCK |
                             GX2R_RESOURCE_USAGE_CPU_READ |
                             GX2R_RESOURCE_USAGE_CPU_WRITE |
                             GX2R_RESOURCE_USAGE_GPU_READ;
    boneTransformBuffer.elemSize = 4 * 4 * 4;
    boneTransformBuffer.elemCount = 32; // up to 32 bones
    GX2RCreateBuffer(&boneTransformBuffer);

    extraBuffer.flags = GX2R_RESOURCE_BIND_UNIFORM_BLOCK |
                             GX2R_RESOURCE_USAGE_CPU_READ |
                             GX2R_RESOURCE_USAGE_CPU_WRITE |
                             GX2R_RESOURCE_USAGE_GPU_READ;
    extraBuffer.elemSize = 4 * 4; // extra uniforms are vec4
    extraBuffer.elemCount = 1; // up to 4 of them
    GX2RCreateBuffer(&extraBuffer);
  }

  void setAttribBuffer(BufferType bt, const void* data, uint32_t elemSize, size_t elemCount) {
    GX2RBuffer* buffer;
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
    setAttribBuffer(data, elemSize, elemCount, buffer);
  }

  void setAttribBuffer(const void* data, uint32_t elemSize, size_t elemCount, GX2RBuffer* buffer) {
    if(!GX2RBufferExists(buffer)) {
      buffer->flags = static_cast<GX2RResourceFlags>(
        GX2R_RESOURCE_BIND_VERTEX_BUFFER |
        GX2R_RESOURCE_USAGE_CPU_READ |
        GX2R_RESOURCE_USAGE_CPU_WRITE |
        GX2R_RESOURCE_USAGE_GPU_READ);
      buffer->elemSize = elemSize;
      buffer->elemCount = elemCount;
      GX2RCreateBuffer(buffer);
    }
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
    } else if (UniformType::CAMERA_VIEW==bt) {
      buffer = &viewBuffer;      
    } else if (UniformType::BONE_TRANSFORM==bt) {
      buffer = &boneTransformBuffer;
    } else {
      WHBLogPrintf("Not a matrix uniform type");
      return;
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

  void setExtraUniform(int index, glm::vec4 data) {
    if (index >= 4) {
      WHBLogPrintf("Extra uniform index out of range");
      return;
    }
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, (void *)&extraBuffer, 4 * 4);
    float* bufferData = (float*)GX2RLockBufferEx(&extraBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, (void *)&extraBuffer, 4 * 4);
    swap_memcpy(bufferData, glm::value_ptr(data), 4 * 4);
    GX2RUnlockBufferEx(&extraBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  }
      

  void render() {
    void *buffer = NULL;

    material->renderUsing();

    if(material->getBindingForBuffer(BufferType::VERTEX) != -1) {
        GX2RSetAttributeBuffer(&positionBuffer, material->getBindingForBuffer(BufferType::VERTEX), positionBuffer.elemSize, 0);
    }
    if(material->getBindingForBuffer(BufferType::COLOR) != -1) {
        GX2RSetAttributeBuffer(&colourBuffer, material->getBindingForBuffer(BufferType::COLOR), colourBuffer.elemSize, 0);
    }
    if(material->getBindingForBuffer(BufferType::TEXCOORD) != -1) {
        GX2RSetAttributeBuffer(&texcoordBuffer, material->getBindingForBuffer(BufferType::TEXCOORD), texcoordBuffer.elemSize, 0);
    }
    if(material->getBindingForBuffer(BufferType::NORMAL) != -1) {
        GX2RSetAttributeBuffer(&normalBuffer, material->getBindingForBuffer(BufferType::NORMAL), normalBuffer.elemSize, 0);
    }
    if(material->getBindingForBuffer(BufferType::BONE_IDX) != -1) {
        GX2RSetAttributeBuffer(&boneIdxBuffer, material->getBindingForBuffer(BufferType::BONE_IDX), boneIdxBuffer.elemSize, 0);
    }
    if(material->getBindingForBuffer(BufferType::BONE_WEIGHT) != -1) {
        GX2RSetAttributeBuffer(&boneWeightBuffer, material->getBindingForBuffer(BufferType::BONE_WEIGHT), boneWeightBuffer.elemSize, 0);
    }
    GX2RSetVertexUniformBlock(&projectionBuffer, 0, 0);
    GX2RSetVertexUniformBlock(&transformBuffer, 1, 0);
    GX2RSetVertexUniformBlock(&boneTransformBuffer, 2, 0);
    GX2RSetVertexUniformBlock(&viewBuffer, 3, 0);
    GX2RSetVertexUniformBlock(&extraBuffer, 4, 0);

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
    GX2RDestroyBufferEx(&boneTransformBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&extraBuffer, GX2R_RESOURCE_BIND_NONE);
  }
};

RenderObject::RenderObject() {
  _impl = new RenderObjectImpl();
}

RenderObject::~RenderObject() {
  delete _impl;
}


void RenderObject::render() { _impl -> render(); }
void RenderObject::setAttribBuffer(BufferType bt, const void* data, uint32_t elemSize, uint32_t elemCount)  { 
  _impl->setAttribBuffer(bt, data, elemSize, elemCount);
}
void RenderObject::setUniformFloatMat(UniformType bt, const float* mat, size_t numFloats) {
  _impl->setUniformFloatMat(bt, mat, numFloats);
}
void RenderObject::setExtraUniform(int index, glm::vec4 data) { 
  _impl->setExtraUniform(index, data);
}
void RenderObject::setMaterial(RenderMaterial* material) {_impl->material = material;}
RenderMaterial* RenderObject::getMaterial() { return _impl->material;}