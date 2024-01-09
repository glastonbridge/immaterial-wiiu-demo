#include "SceneObject.h"
#include "Material.h"
#include <memory>
#include <malloc.h>
#include "../math/projection.h"
#include <gx2/enum.h>

static const float sPositionData[] =
{
   // front
   -1.0f, 1.0f,  0.0f,
    1.0f,  1.0f,  0.0f,
   -1.0f, -1.0f,  0.0f,
   
    1.0f,  1.0f,  0.0f,
   -1.0f, -1.0f,  0.0f,
   1.0f,  -1.0f,  0.0f,

   -1.0f+2.2f, 1.0f,  0.0f,
    1.0f+2.2f,  1.0f,  0.0f,
   -1.0f+2.2f, -1.0f,  0.0f,
   
    1.0f+2.2f,  1.0f,  0.0f,
   -1.0f+2.2f, -1.0f,  0.0f,
    1.0f+2.2f,  -1.0f,  0.0f,

   -1.0f+2.2f, 1.0f+2.2f,  0.0f,
    1.0f+2.2f,  1.0f+2.2f,  0.0f,
   -1.0f+2.2f, -1.0f+2.2f,  0.0f,
   
    1.0f+2.2f,  1.0f+2.2f,  0.0f,
   -1.0f+2.2f, -1.0f+2.2f,  0.0f,
    1.0f+2.2f,  -1.0f+2.2f,  0.0f,

   -1.0f, 1.0f+2.2f,  0.0f,
    1.0f,  1.0f+2.2f,  0.0f,
   -1.0f, -1.0f+2.2f,  0.0f,
   
    1.0f,  1.0f+2.2f,  0.0f,
   -1.0f, -1.0f+2.2f,  0.0f,
   1.0f,  -1.0f+2.2f,  0.0f,
};

static const float sColourData[] =
{
   0.1f,  0.1f,  1.0f, 1.0f,
   0.1f,  0.1f,  1.0f, 1.0f,
   0.1f,  0.1f,  1.0f, 1.0f,
   0.1f,  0.1f,  1.0f, 1.0f,
   0.1f,  0.1f,  1.0f, 1.0f,
   0.1f,  0.1f,  1.0f, 1.0f,

   1.0f,  1.0f,  0.0f, 1.0f,
   1.0f,  1.0f,  0.0f, 1.0f,
   1.0f,  1.0f,  0.0f, 1.0f,
   1.0f,  1.0f,  0.0f, 1.0f, 
   1.0f,  1.0f,  0.0f, 1.0f,
   1.0f,  1.0f,  0.0f, 1.0f, 

   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,
   0.0f,  1.0f,  0.0f, 1.0f,

   1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  0.0f,  0.0f, 1.0f,
   1.0f,  0.0f,  0.0f, 1.0f,
};

/**
 * Currently totally fake, but this should be an entry point to load
 * a 3D object from a path. Instead, it creates a quad
 */
std::unique_ptr<SceneObject> LoadObject(const char* path) {
  // TODO: Construct the object from an object file (assimp?)
  std::unique_ptr<SceneObject> _impl;
  _impl.reset(new SceneObject());
  Material* material(new ProjectedMaterial());
  _impl->setMaterial(material);
  _impl->setPositionBuffer(sPositionData, 4*3, 6*4);
  _impl->setColourBuffer(sColourData, 4*4, 6*4);

   // TODO: who the fuck owns this? Leaks all over the shop because
   // I haven't got enough working to care about structure yet
   float* projectionBuffer = (float*) memalign(GX2_UNIFORM_BLOCK_ALIGNMENT, sizeof(float)*16);

   calculateProjectionMatrix(projectionBuffer, 0.6, 16.0f/9.0f, 0.1, 2000);
   _impl->setProjectionBuffer(projectionBuffer);
  return _impl;
}
