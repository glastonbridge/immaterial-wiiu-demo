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

#include "CafeGLSLCompiler.h"

#include "../scenes/SceneBase.h"
#include "RenderObject.h"
#include <glm/ext.hpp>
#include "RenderMaterial.h"

Renderer::Renderer()
{
   WHBGfxInit();
   GLSL_Init();
   renderBuffer = new BufferTexture();
}

void Renderer::renderFrame(const SceneBase& scene) {
      // Render!
      WHBLogPrint("Rendering frame...");

      WHBLogPrint("Binding render target");
      renderBuffer->bindTarget(true);
      float* cameraProjection = (float*)glm::value_ptr(scene.cameraProjection);

      for (auto& object : scene.objects) {
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_PROJECTION, cameraProjection, 16);
         object->getRenderObject()->render();
      }
      WHBLogPrint("Rendered one");
      renderBuffer->unbindTarget();

      WHBLogPrint("Unbound render target");
      
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);

      for (auto& object : scene.objects) {
         renderBuffer->renderUsing(object->getRenderObject()->getMaterial()->group);
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_PROJECTION, cameraProjection, 16);
         object->getRenderObject()->render();
      }
      WHBLogPrint("Finished TV");

      WHBGfxFinishRenderTV();
      
      WHBGfxBeginRenderDRC();

      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      for (auto& object : scene.objects) {
         renderBuffer->renderUsing(object->getRenderObject()->getMaterial()->group);
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_PROJECTION, cameraProjection, 16);
         object->getRenderObject()->render();
      }

      WHBGfxFinishRenderDRC();
      WHBLogPrint("Finished DRC");

      WHBGfxFinishRender();
      WHBLogPrint("Finished a render");
   }

Renderer::~Renderer() {
   
   WHBLogPrintf("Exiting...");
   WHBGfxShutdown();
}
