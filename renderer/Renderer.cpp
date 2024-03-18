#include "Renderer.h"
#include <gfd.h>
#include <gx2/draw.h>
#include <gx2/shaders.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2r/draw.h>
#include <gx2r/buffer.h>
#include <gx2/state.h>
#include <gx2/event.h>
#include <string.h>
#include <stdio.h>
#include <whb/file.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <cstdlib>

#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>

#include "../scenes/SceneBase.h"
#include "RenderObject.h"
#include <glm/ext.hpp>
#include "RenderMaterial.h"

Renderer::Renderer()
{
   fullscreenQuad = LoadQuad();
}

void Renderer::renderFrame(const SceneBase& scene) {
      // Render to offscreen buffer
      WHBLogPrint("Binding render target");
      scene.renderBuffer->bindTarget(true);

      float* cameraProjection = (float*)glm::value_ptr(scene.cameraProjection);

      for (auto& object : scene.objects) {
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_PROJECTION, (float*)glm::value_ptr(scene.cameraProjection), 16);
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_VIEW, (float*)glm::value_ptr(scene.cameraView), 16);
         object->getRenderObject()->render();
      }
      scene.renderBuffer->unbindTarget();

      //  Render to screen with post processing
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      scene.renderBuffer->renderUsing(fullscreenQuad->getRenderObject()->getMaterial()->group);
      fullscreenQuad->getRenderObject()->render();
      WHBGfxFinishRenderTV();
      
      WHBGfxBeginRenderDRC();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      scene.renderBuffer->renderUsing(fullscreenQuad->getRenderObject()->getMaterial()->group);
      fullscreenQuad->getRenderObject()->render();
      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

Renderer::~Renderer() {
   WHBLogPrintf("Exiting...");
}
