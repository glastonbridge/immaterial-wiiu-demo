#include "SceneObject.h"
#include "Material.h"
#include <memory>

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




/**
 * Currently totally fake, but this should be an entry point to load
 * a 3D object from a path. Instead, it sets up the default billboard
 * from Vurpo's code.
 */
SceneObject* LoadObject(const char* path) {
  // TODO: Construct the object from an object file (assimp?)
  
  SceneObject* _impl = new SceneObject();
  Material* material = new Material();
  material->attachShaders();
  _impl->setMaterial(material);
  _impl->setPositionBuffer(sPositionData, 4*4, 3);
  _impl->setColourBuffer(sPositionData, 4*4, 3);
  return _impl;
}