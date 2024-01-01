#include <gfd.h>
#include <string.h>
#include <stdio.h>
#include <whb/file.h>
#include <whb/proc.h>
#include <whb/sdcard.h>
#include <whb/gfx.h>
#include <whb/log.h>
#include <whb/log_cafe.h>
#include <cstdlib>

#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>

#include "scene/TestScene.h"
#include "renderer/Renderer.h"


int main(int argc, char **argv)
{

   WHBLogCafeInit();
   WHBProcInit();
   WHBLogPrint("Hello World! Logging initialised.");
   Renderer renderer;
   TestScene scene;
   scene.setup();
   WHBLogPrintf("Begin updating...");
   while (WHBProcIsRunning()) {
      renderer.renderFrame(scene);
   }

exit:
   WHBUnmountSdCard();
   WHBProcShutdown();
   return 0;
}
