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
#include "../graphics/MaterialCollection.h"
#include "../util/memory.h"

Renderer::Renderer()
{
   composeQuad = LoadQuad(new ComposeMaterial());
   blurQuad = LoadQuad(new BlurMaterial());
   bufferA = std::make_unique<RenderBuffer>(false, 1280, 720);
   bufferB = std::make_unique<RenderBuffer>(false, 1280, 720);
}

int32_t GX2GetVertexUniformVarOffset(const GX2VertexShader *shader, const char *name)
{
	GX2UniformVar *uniform = GX2GetVertexUniformVar(shader, name);
	return uniform ? uniform->offset : -1;
}

int32_t GX2GetPixelUniformVarOffset(const GX2PixelShader *shader, const char *name)
{
	GX2UniformVar *uniform = GX2GetPixelUniformVar(shader, name);
	return uniform ? uniform->offset : -1;
}


int32_t GX2GetPixelUniformVarType(const GX2PixelShader *shader, const char *name)
{
	GX2UniformVar *uniform = GX2GetPixelUniformVar(shader, name);
	return uniform ? uniform->type : -1;
}

static float uniformData[512 * 4] __attribute__ ((aligned (0x40)));

void Renderer::renderFrame(const SceneBase& scene) {
      // Render to offscreen buffer
      //WHBLogPrint("Binding render target");
      scene.renderBuffer->bindTarget(true);

      float* cameraProjection = (float*)glm::value_ptr(scene.cameraProjection);

      for (auto& object : scene.objects) {
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_PROJECTION, (float*)glm::value_ptr(scene.cameraProjection), 16);
         object->getRenderObject()->setUniformFloatMat(UniformType::CAMERA_VIEW, (float*)glm::value_ptr(scene.cameraView), 16);
         object->getRenderObject()->setExtraUniform(0, glm::vec4(syncVal("Camera:FocalDist"), syncVal("Camera:FocalLen"), syncVal("Camera:Aperture"), syncVal("Global:FresnelPow")));
         object->getRenderObject()->render();
      }
      scene.renderBuffer->unbindTarget();

      // Apply blur
      for(int i = 0; i < 2; i++) {
         float scale = (float)(1 << (i));
         
         // Horizontal
         bufferA->bindTarget(true);
         if (i == 0) {
            scene.renderBuffer->renderUsing(blurQuad->getRenderObject()->getMaterial()->group);
         }
         else {
            bufferB->renderUsing(blurQuad->getRenderObject()->getMaterial()->group);
         }
         blurQuad->getRenderObject()->setExtraUniform(0, glm::vec4(scale, 0.0001f, 0.0f, 0.0f));
         blurQuad->getRenderObject()->render();
         bufferA->unbindTarget();

         // Vertical
         bufferB->bindTarget(true);
         bufferA->renderUsing(blurQuad->getRenderObject()->getMaterial()->group);
         blurQuad->getRenderObject()->setExtraUniform(0, glm::vec4(scale, 1.0001f, 0.0f, 0.0f));
         blurQuad->getRenderObject()->render();
         bufferB->unbindTarget();
      }

      //  Render to screen with post processing
      WHBGfxBeginRender();
      WHBGfxBeginRenderTV();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);      
      bufferB->renderUsing(composeQuad->getRenderObject()->getMaterial()->group);
      composeQuad->getRenderObject()->render();      
      WHBGfxFinishRenderTV();
      
      WHBGfxBeginRenderDRC();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      bufferB->renderUsing(composeQuad->getRenderObject()->getMaterial()->group);
      composeQuad->getRenderObject()->render();
      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

Renderer::~Renderer() {
   WHBLogPrintf("Exiting...");
}
