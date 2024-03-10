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
#include <coreinit/filesystem.h>

#include <sndcore2/core.h>

#include "scenes/TestScene.h"
#include "renderer/Renderer.h"
#include <whb/log_udp.h>
#include "sound/Music.h"
#include "sync/Sync.h"

int main(int argc, char **argv)
{

   WHBLogCafeInit();
   WHBLogUdpInit();
   WHBProcInit();
   // TODO: see loadShader in Material.cpp
   WHBMountSdCard();
   WHBLogPrint("Hello World! Logging initialised.");
   {
      MusicPlayer music("assets/dumdumdiday.ogg");
      Sync sync("sync_tracks/", SYNC_IP, &music, 0.1f);
      syncObjPtr = &sync;
      Renderer renderer;
      TestScene scene;
      scene.setup();
      WHBLogPrintf("Begin updating...");
      music.play();
      while (WHBProcIsRunning()) {
         sync.update();
         scene.update(music.currentTime());
         renderer.renderFrame(scene);
         
         WHBLogPrintf("Frame done, playback time is %f", music.currentTime());
      }
   }
   WHBLogPrintf("Done. Quitting...");  
   AXQuit();
   WHBUnmountSdCard();
   WHBLogCafeDeinit();
   WHBProcShutdown();
   WHBLogUdpDeinit();
   return 0;
}
