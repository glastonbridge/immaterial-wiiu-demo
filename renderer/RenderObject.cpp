#include "RenderObject.h"
#include "../graphics/Material.h"

#include <dmae/mem.h>

#include <gx2/shaders.h>
#include <gx2r/buffer.h>
#include <gx2/draw.h>
#include <gx2r/draw.h>

#include <whb/gfx.h>
#include <whb/log.h>
#include <gx2/mem.h>
#include "../math/projection.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct RenderObjectImpl {
  Material* material;

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
      buffer->flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
                      GX2R_RESOURCE_USAGE_CPU_READ |
                      GX2R_RESOURCE_USAGE_CPU_WRITE |
                      GX2R_RESOURCE_USAGE_GPU_READ;
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
  void setProjectionBuffer(const float *data) // <- ignored currently
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

    GX2RCreateBuffer(&projectionBuffer);

    buffer = GX2RLockBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
    //memcpy(buffer, data, projectionBuffer.elemSize * projectionBuffer.elemCount);
    DMAEWaitDone(DMAECopyMem(buffer, data, projectionBuffer.elemSize * projectionBuffer.elemCount / 4, DMAE_SWAP_32)); // <-- this works, with possible caveats: might have sync issues with GPU, might be expensive for just a single matrix (but maybe worth it for e.g. set of 20 bone mats or such)

    GX2RUnlockBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  }

  void render() {
    WHBGfxClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    material->renderUsing();

    GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
    GX2RSetAttributeBuffer(&texcoordBuffer, 2, texcoordBuffer.elemSize, 0);
    GX2RSetVertexUniformBlock(&projectionBuffer, 0, 0);

    GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, positionBuffer.elemCount, 0, 1);

  }
  ~RenderObjectImpl() {
      
    GX2RDestroyBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
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
void RenderObject::setMaterial(Material* material) {_impl->material = material;}