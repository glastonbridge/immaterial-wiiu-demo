#include "Object.h"
#include "Material.h"

#include <gx2/shaders.h>
#include <gx2r/buffer.h>
#include <gx2/draw.h>
#include <gx2r/draw.h>

#include <whb/gfx.h>
#include <whb/log.h>

static const float sPositionData[] =
{
    1.0f, -1.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  0.0f, 1.0f,
   -1.0f, -1.0f,  1.0f, 1.0f,
};

static const float sColourData[] =
{
   1.0f,  0.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  0.0f,  1.0f, 1.0f,
};


struct ObjectImpl: public Object {
  Material material;

   GX2RBuffer positionBuffer = {};
   GX2RBuffer colourBuffer = {};

  void render() override {
      WHBGfxClearColor(0.0f, 0.0f, 1.0f, 1.0f);

      material.renderUsing();

      GX2RSetAttributeBuffer(&positionBuffer, 0, positionBuffer.elemSize, 0);
      GX2RSetAttributeBuffer(&colourBuffer, 1, colourBuffer.elemSize, 0);
      GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);

  }
  ~ObjectImpl() {
      
    GX2RDestroyBufferEx(&positionBuffer, GX2R_RESOURCE_BIND_NONE);
    GX2RDestroyBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
  }
};


/**
 * Currently totally fake, but this should be an entry point to load
 * a 3D object from a path. Instead, it sets up the default billboard
 * from Vurpo's code.
 */
Object* LoadObject(const char* path) {
  // TODO: Construct the object from an object file (assimp?)
  
    WHBLogPrintf("Loading an object");
  ObjectImpl* impl = new ObjectImpl();
  impl->material.attachShaders();


   void *buffer = NULL;

   // Set vertex position
   impl->positionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
                          GX2R_RESOURCE_USAGE_CPU_READ |
                          GX2R_RESOURCE_USAGE_CPU_WRITE |
                          GX2R_RESOURCE_USAGE_GPU_READ;
   impl->positionBuffer.elemSize = 4 * 4;
   impl->positionBuffer.elemCount = 3;
   GX2RCreateBuffer(&impl->positionBuffer);
   buffer = GX2RLockBufferEx(&impl->positionBuffer, GX2R_RESOURCE_BIND_NONE);
   memcpy(buffer, sPositionData, impl->positionBuffer.elemSize * impl->positionBuffer.elemCount);
   GX2RUnlockBufferEx(&impl->positionBuffer, GX2R_RESOURCE_BIND_NONE);

   // Set vertex colour
   impl->colourBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
                        GX2R_RESOURCE_USAGE_CPU_READ |
                        GX2R_RESOURCE_USAGE_CPU_WRITE |
                        GX2R_RESOURCE_USAGE_GPU_READ;
   impl->colourBuffer.elemSize = 4 * 4;
   impl->colourBuffer.elemCount = 3;
   GX2RCreateBuffer(&impl->colourBuffer);
   buffer = GX2RLockBufferEx(&impl->colourBuffer, GX2R_RESOURCE_BIND_NONE);
   memcpy(buffer, sColourData, impl->colourBuffer.elemSize * impl->colourBuffer.elemCount);
   GX2RUnlockBufferEx(&impl->colourBuffer, GX2R_RESOURCE_BIND_NONE);

  return impl;
}