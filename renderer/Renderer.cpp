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
   
      // Animate colours...
      /*
      float *colours = (float *)GX2RLockBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
      colours[0] = 1.0f;
      colours[1] = colours[1] >= 1.0f ? 0.0f : (colours[1] + 0.01f);
      colours[2] = colours[2] >= 1.0f ? 0.0f : (colours[2] + 0.01f);
      colours[3] = 1.0f;

      colours[4] = colours[4] >= 1.0f ? 0.0f : (colours[4] + 0.01f);
      colours[5] = 1.0f;
      colours[6] = colours[6] >= 1.0f ? 0.0f : (colours[6] + 0.01f);
      colours[7] = 1.0f;

      colours[8] = colours[8] >= 1.0f ? 0.0f : (colours[8] + 0.01f);
      colours[9] = colours[9] >= 1.0f ? 0.0f : (colours[9] + 0.01f);
      colours[10] = 1.0f;
      colours[11] = 1.0f;
      GX2RUnlockBufferEx(&colourBuffer, GX2R_RESOURCE_BIND_NONE);
*/
      // Render!
      WHBGfxBeginRender();

      WHBGfxBeginRenderTV();
      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);

      for (RenderObject* object : scene.objects) {
         object->render();
      }


      WHBGfxFinishRenderTV();

      WHBGfxBeginRenderDRC();

      WHBGfxClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      for (RenderObject* object : scene.objects) {
         object->render();
      }

      WHBGfxFinishRenderDRC();

      WHBGfxFinishRender();
   }

Renderer::~Renderer() {
   
   WHBLogPrintf("Exiting...");
   WHBGfxShutdown();
}
