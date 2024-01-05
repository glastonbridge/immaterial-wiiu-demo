#include "Renderer.h"
#include <gfd.h>
#include <gx2/draw.h>
#include <gx2/shaders.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2r/draw.h>
#include <gx2r/buffer.h>
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

Renderer::Renderer()
{
   int result = 0;

   WHBGfxInit();
   GLSL_Init();

   // TODO: Multiple shaders


   WHBLogPrintf("Begin rendering...");
}

void Renderer::renderFrame(const SceneBase& scene) {
         // Render!
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);

      for (auto& object : scene.objects) {
         object->render();
      }


      WHBGfxFinishRenderTV();

      WHBGfxBeginRenderDRC();

      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      for (auto& object : scene.objects) {
         object->render();
      }

      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

Renderer::~Renderer() {
   
   WHBLogPrintf("Exiting...");
   WHBGfxShutdown();
}
