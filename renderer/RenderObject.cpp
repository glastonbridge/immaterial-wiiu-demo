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
  GX2RBuffer projectionBuffer = {};

  void setPositionBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {

    WHBLogPrintf("Loading an object");


   void *buffer = NULL;

   // Set vertex position
   positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
                          GX2R_RESOURCE_USAGE_CPU_READ |
                          GX2R_RESOURCE_USAGE_CPU_WRITE |
                          GX2R_RESOURCE_USAGE_GPU_READ;
   positionBuffer.elemSize = elemSize;
   positionBuffer.elemCount = elemCount;
   GX2RCreateBuffer(&positionBuffer);
   buffer = GX2RLockBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
   memcpy(buffer, data, positionBuffer.elemSize * positionBuffer.elemCount);
   GX2RUnlockBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
  }

  void setColourBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
    
   void *buffer = NULL;
   // Set vertex colour
   colourBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
                        GX2R_RESOURCE_USAGE_CPU_READ |
                        GX2R_RESOURCE_USAGE_CPU_WRITE |
                        GX2R_RESOURCE_USAGE_GPU_READ;
   colourBuffer.elemSize = elemSize;
   colourBuffer.elemCount = elemCount;
   GX2RCreateBuffer(&colourBuffer);
   buffer = GX2RLockBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
   memcpy(buffer, data, colourBuffer.elemSize * colourBuffer.elemCount);
   GX2RUnlockBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);

  }

  /**
   * HELP ME FRIENDS
   * 
   * I can't get uniforms to pass through to the vertex shader.
   * 
   * If you look at shaders/projected.vert you will see some attempt to read in the projection
   * buffer. As it stands, it has a hacked projection matrix, and it sets the green value to 
   * [0,0] in the projection matrix passed in.
   * 
   * It should be green, because [0,0] is about 0.8. It's black, because the projection matrix
   * is not getting passed in.
   * 
   * I have made a couple of attempts, one to pass it in as a buffer, and another is to use
   * the built-in SetVertexUniformBlock commands. Neither is working.
   * 
   * I'm pretty sure this is one of those cases where having a working example would solve this
   * problem really quickly.
   * 
   * Good hunting
   * 
   *  -aldroid
   */
  /**
   * Hi! This works now, but as you can see at the beginning of the setProjectionBuffer function,
   * I'm not actually swapping the byte order when copying the memory, just fudging this first value
   * to prove it works. Have fun!
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
    auto mat = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 10.f) * glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -5.f)) * glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
    data = (float*)glm::value_ptr(mat);

    WHBLogPrintf("Projection %f,%f,%f,%f,", data[0], data[1], data[2], data[3]);
    WHBLogPrintf("Projection %f,%f,%f,%f,", data[4], data[5], data[6], data[7]);
    WHBLogPrintf("Projection %f,%f,%f,%f,", data[8], data[9], data[10], data[11]);
    WHBLogPrintf("Projection %f,%f,%f,%f,", data[12], data[13], data[14], data[15]);
    GX2RCreateBuffer(&projectionBuffer);

    buffer = GX2RLockBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
    //memcpy(buffer, data, projectionBuffer.elemSize * projectionBuffer.elemCount);
    DMAEWaitDone(DMAECopyMem(buffer, data, projectionBuffer.elemSize * projectionBuffer.elemCount / 4, DMAE_SWAP_32)); // <-- this works, with possible caveats: might have sync issues with GPU, might be expensive for just a single matrix (but maybe worth it for e.g. set of 20 bone mats or such)

    GX2RUnlockBufferEx(&projectionBuffer, GX2R_RESOURCE_BIND_UNIFORM_BLOCK);
  }

  void render() {
      WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);

      material->renderUsing();

      GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      GX2RSetVertexUniformBlock(&projectionBuffer,
                                0, 0);

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
void RenderObject::setProjectionBuffer(const float* data)  { _impl->setProjectionBuffer(data);};
void RenderObject::setMaterial(Material* material) {_impl->material = material;}