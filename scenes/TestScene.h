#include "SceneBase.h"

#include "../sync/Sync.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../renderer/RenderObject.h"

#include "../graphics/SceneObject.h"

struct TestScene: public SceneBase {
  void setup() override {
    
    objects.push_back(LoadObject("assets/gizmos.fbx"));
  }

  void update(double time) override {
    cameraProjection = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 20.f) * \
      glm::lookAt(
        glm::vec3(0.0, -10.0, 0.0001), 
        glm::vec3(0.f, 0.f, 0.f), 
        glm::vec3(0.f, 1.f, 0.f)
      );

    auto transform = glm::rotate(glm::mat4(1.f), glm::radians(syncVal("TestPart:Object:RotY")), glm::vec3(0.f, 1.f, 0.f));
    
    float* mat = (float*)glm::value_ptr(transform);
    objects[0]->getRenderObject()->setUniformFloatMat(UniformType::TRANSFORM, mat, 16);

    size_t numBones = objects[0]->animFrames[0].size();
    float* boneMatsForFrameInterp = (float*)malloc(4*3*numBones*sizeof(float)); // TODO permanentize, fuck mallocing every frame
    size_t frame = syncVal("TestPart::Object:Frame");
    frame = std::min(frame, objects[0]->animFrames.size());
    frame = std::max(frame, 0u);
    
    // Copy all the bone mats into the array. TODO interpolate
    for(int i = 0; i < numBones; i++) {
      memcpy(boneMatsForFrameInterp + (i * 4 * 3), glm::value_ptr(objects[0]->animFrames[frame][i]), 4*3*sizeof(float));
    }

    objects[0]->getRenderObject()->setUniformFloatMat(UniformType::BONE_TRANSFORM, boneMatsForFrameInterp, 4*3*numBones); // 4x3 matrix, 2 bones
    free(boneMatsForFrameInterp);
  }
};
