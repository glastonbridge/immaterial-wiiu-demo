
#include "SceneMaterial.h"
#include <memory>
#include <malloc.h>
#include <gx2/enum.h>
#include "NastyObjLoader.h"
#include "LoadUFBX.h"

#include "../renderer/RenderObject.h"
#include "SceneObject.h"

/**
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
*/

SceneObject::SceneObject() {

}

SceneObject::~SceneObject() {
   if(this->boneMatInterpBuffer != nullptr) {
      free(this->boneMatInterpBuffer);
   }
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

void SceneObject::setAnimationFrame(float frame) {
    // Bail if we don't have any frames
    if (this->animFrames.size() == 0) {
        return;
    }
    
    // Alloc buffer if we need one
    size_t numBones = this->animFrames[0].size();
    if(this->boneMatInterpBuffer == nullptr) {
      this->boneMatInterpBuffer = (float*)malloc(4 * 4 * numBones * sizeof(float));
    }

    // Figure out where in the animation we are
    size_t numFrames = this->animFrames.size();
    int animPos = (int)frame;
    float animPosRemainder = frame - (float)animPos;
    animPos = animPos % numFrames;
    int animPosNext = (animPos + 1) % numFrames;

    // Copy all the bone mats into the array, interpolating linearly between two adjacent frames
    for(int i = 0; i < numBones; i++) {
      glm::mat4 boneFrameMat = this->animFrames[animPos][i] * (1.0f - animPosRemainder) + this->animFrames[animPosNext][i] * animPosRemainder;
      memcpy(this->boneMatInterpBuffer + (i * 4 * 4), glm::value_ptr(boneFrameMat), 4 * 4 * sizeof(float));
    }

    // Bones to shader buffer
    this->getRenderObject()->setUniformFloatMat(UniformType::BONE_TRANSFORM, this->boneMatInterpBuffer, 4 * 4 * numBones);
}

/**
 * Currently extremely real
 */
std::unique_ptr<SceneObject> LoadObject(const char* path, const char* name) {

    std::unique_ptr<SceneObjectImpl> _impl;
    _impl.reset(new SceneObjectImpl());

    SceneMaterial* material(new BoneMaterial("assets/colorgrid.png"));
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<float> normals;
    std::vector<float> boneIndices;
    std::vector<float> boneWeights;

    LoadUFBX(path, name, vertices, texcoords, normals, boneIndices, boneWeights, _impl->animFrames);
    
    std::vector<float> vertexColors;
    for (uint32_t i=0; i < normals.size()*4/3; ++i) {
        vertexColors.push_back(0);
    }

    _impl->setMaterial(material);
    _impl->getRenderObject()->setAttribBuffer(BufferType::VERTEX, vertices.data(), 4*3, vertices.size()/3);
    _impl->getRenderObject()->setAttribBuffer(BufferType::COLOR, vertexColors.data(), 4*4, vertexColors.size()/4);
    _impl->getRenderObject()->setAttribBuffer(BufferType::TEXCOORD, texcoords.data(), 4*2, texcoords.size()/2);
    _impl->getRenderObject()->setAttribBuffer(BufferType::NORMAL, normals.data(), 4*3, normals.size()/3);
    _impl->getRenderObject()->setAttribBuffer(BufferType::BONE_IDX, boneIndices.data(), 2*4, boneIndices.size()/2);
    _impl->getRenderObject()->setAttribBuffer(BufferType::BONE_WEIGHT, boneWeights.data(), 2*4, boneWeights.size()/2);

    return _impl;
}
