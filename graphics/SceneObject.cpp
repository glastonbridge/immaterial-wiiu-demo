
#include "SceneMaterial.h"
#include <memory>
#include <malloc.h>
#include <gx2/enum.h>

#include "../renderer/RenderObject.h"
#include "SceneObject.h"

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

static const float sTexcoordData[] =
{
    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};

SceneObject::SceneObject() {

}

SceneObject::~SceneObject() {
   
}

RenderObject* SceneObject::getRenderObject() {
   return renderObject.get();
}

struct SceneObjectImpl: public SceneObject {
   SceneObjectImpl() {
      renderObject.reset(new RenderObject());
   }
   void setMaterial(SceneMaterial* material) {
      sceneMaterial.reset(material); // memory owned by the scene now
      renderObject->setMaterial(material->getRenderMaterial());
   }
   void setPositionBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
      renderObject->setPositionBuffer(data,elemSize,elemCount);
   }
   void setColourBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
      renderObject->setColourBuffer(data,elemSize,elemCount);
   }
   void setTexcoordBuffer(const float* data, uint32_t elemSize, uint32_t elemCount) {
      renderObject->setTexcoordBuffer(data, elemSize, elemCount);
   }
   void setProjectionBuffer(const float* data) {
      renderObject->setProjectionBuffer(data);
   }
protected:
   std::unique_ptr<SceneMaterial> sceneMaterial;
};

/**
 * Currently totally fake, but this should be an entry point to load
 * a 3D object from a path. Instead, it creates a quad
 */
std::unique_ptr<SceneObject> LoadObject(const char* path) {
  // TODO: Construct the object from an object file (assimp?)
  std::unique_ptr<SceneObjectImpl> _impl;
  _impl.reset(new SceneObjectImpl());
  SceneMaterial* material(new TextureMaterial("assets/molcar.png"));
  //SceneMaterial* material(new ProjectedMaterial());
  _impl->setMaterial(material);
  _impl->setPositionBuffer(sPositionData, 4*3, 6*4);
  _impl->setColourBuffer(sColourData, 4*4, 6*4);
  _impl->setTexcoordBuffer(sTexcoordData, 4*2, 6*4);

   // TODO: who the fuck owns this? Leaks all over the shop because
   // I haven't got enough working to care about structure yet
   float* projectionBuffer = (float*) memalign(GX2_UNIFORM_BLOCK_ALIGNMENT, sizeof(float)*16);
   _impl->setProjectionBuffer(projectionBuffer);
  return _impl;
}
