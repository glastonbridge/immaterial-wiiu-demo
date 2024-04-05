#include "SceneObject.h"
#include <gx2/enum.h>
#include <memory>

#include "../renderer/RenderObject.h"
#include <whb/log.h>

void SceneObject::applyAnimation(RenderObject &renderObject) const {
  // Bail if we don't have any frames
  if (renderObject.animFrames.empty()) {
    glm::mat4 boneFrameMat = glm::mat4(1.0f);
    renderObject.setUniformFloatMat(
        UniformType::BONE_TRANSFORM, glm::value_ptr(boneFrameMat), 4 * 4);
    return;
  }

  // Alloc buffer if we need one
  size_t numBones = renderObject.animFrames[0].size();
  if (this->boneMatInterpBuffer == nullptr) {
    WHBLogPrintf("Allocating bone interp buffer for %d bones", numBones);
    this->boneMatInterpBuffer.reset(new float[4 * 4 * numBones]);
  }
  renderObject.getAnimFrame(anim, boneMatInterpBuffer.get());

  // Bones to shader buffer
  WHBLogPrintf("Setting bone transform uniform");
  renderObject.setUniformFloatMat(
      UniformType::BONE_TRANSFORM, boneMatInterpBuffer.get(), 4 * 4 * numBones);
}
