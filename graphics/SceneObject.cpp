
#include "SceneMaterial.h"
#include <memory>
#include <malloc.h>
#include <gx2/enum.h>
#include "NastyObjLoader.h"

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

  std::vector<float> vertices;
  std::vector<float> texcoords;
  std::vector<float> normals;
  NastyImportObj(path, vertices, texcoords, normals);

   std::vector<float> vertexColors;
   for (uint32_t i=0; i < normals.size()*4/3; ++i) {
      vertexColors.push_back(0);
   }

  _impl->setMaterial(material);
  _impl->getRenderObject()->setAttribBuffer(BufferType::VERTEX, vertices.data(), 4*3, vertices.size()/3);
  _impl->getRenderObject()->setAttribBuffer(BufferType::COLOR, vertexColors.data(), 4*4, vertexColors.size()/4);
  _impl->getRenderObject()->setAttribBuffer(BufferType::TEXCOORD, texcoords.data(), 4*2, texcoords.size()/2);
  _impl->getRenderObject()->setAttribBuffer(BufferType::NORMAL, normals.data(), 4*3, normals.size()/3);

  return _impl;
}
