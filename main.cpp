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

#include <sndcore2/core.h>

#include "scenes/TestScene.h"
#include "renderer/Renderer.h"
#include <whb/log_udp.h>


int main(int argc, char **argv)
{

   WHBLogCafeInit();
   WHBLogUdpInit();
   WHBProcInit();
   AXInit();
   WHBLogPrint("Hello World! Logging initialised.");
   Renderer renderer;
   TestScene scene;
   scene.setup();
   WHBLogPrintf("Begin updating...");
   while (WHBProcIsRunning()) {
      renderer.renderFrame(scene);
   }

exit:
   AXQuit();
   WHBUnmountSdCard();
   WHBProcShutdown();
   WHBLogUdpDeinit();
   WHBLogCafeDeinit();
   return 0;
}
