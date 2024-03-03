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

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct RenderObjectImpl {
  RenderMaterial* material;

  GX2RBuffer positionBuffer = {};
  GX2RBuffer colourBuffer = {};
  GX2RBuffer texcoordBuffer = {};
  GX2RBuffer projectionBuffer = {};

  void setPositionBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
    setAttribBuffer(data, elemSize, elemCount, &positionBuffer);
  }

  void setColourBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
    setAttribBuffer(data, elemSize, elemCount, &colourBuffer);
  }

  void setTexcoordBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
    setAttribBuffer(data, elemSize, elemCount, &texcoordBuffer);
  }

  void setAttribBuffer(const float* data, uint32_t elemSize, uint32_t elemCount, GX2RBuffer* buffer) {
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

  /**
   * Uniforms work now but right now it also just computes matrix right here. TODO: don't do that
  */
  void setProjectionBuffer(const float *data) 
  {
    void *buffer = NULL;
    // Set vertex colour
    projectionBuffer.flags = GX2R_RESOURCE_BIND_UNIFORM_BLOCK |
                             GX2R_RESOURCE_USAGE_CPU_READ |
                             GX2R_RESOURCE_USAGE_CPU_WRITE |
                             GX2R_RESOURCE_USAGE_GPU_READ;
    projectionBuffer.elemSize = 4 * 4 * 4;
    projectionBuffer.elemCount = 1;
    // log
    auto mat = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 20.f) * glm::translate(glm::mat4(1.f), glm::vec3(-0.7f, -0.7f, -10.f)) * glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
    data = (float*)glm::value_ptr(mat);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, (void *)data, 16 * 4);

    GX2RCreateBuffer(&projectionBuffer);

    buffer = GX2RLockBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, (void *)buffer, 16 * 4);
    swap_memcpy(buffer, data, 16 * 4);
    //DMAEWaitDone(DMAECopyMem(buffer, data, projectionBuffer.elemSize * projectionBuffer.elemCount, DMAE_SWAP_32)); // <-- this works, with possible caveats: might have sync issues with GPU, might be expensive for just a single matrix (but maybe worth it for e.g. set of 20 bone mats or such)

    GX2RUnlockBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  }

  void render() {
    material->renderUsing();

    GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&texcoordBuffer, 2, texcoordBuffer.elemSize, 0);
    GX2RSetVertexUniformBlock(&projectionBuffer, 0, 0);

    GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, positionBuffer.elemCount, 0, 1);

  }
  ~RenderObjectImpl() {

    WHBLogPrintf("Destroying RenderObject"); 
    GX2RDestroyBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&texcoordBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_NONE);
  }
};

RenderObject::RenderObject() {
  _impl = new RenderObjectImpl();
}

RenderObject::~RenderObject() {
  delete _impl;
}


void RenderObject::render() { _impl -> render(); }
void RenderObject::setPositionBuffer(const float* data, uint32_t elemSize, uint32_t elemCount)  { _impl->setPositionBuffer(data, elemSize, elemCount);}
void RenderObject::setColourBuffer(const float* data, uint32_t elemSize, uint32_t elemCount)  { _impl->setColourBuffer(data, elemSize, elemCount);};
void RenderObject::setTexcoordBuffer(const float* data, uint32_t elemSize, uint32_t elemCount)  { _impl->setTexcoordBuffer(data, elemSize, elemCount);};
void RenderObject::setProjectionBuffer(const float* data)  { _impl->setProjectionBuffer(data);};
void RenderObject::setMaterial(RenderMaterial* material) {_impl->material = material;}