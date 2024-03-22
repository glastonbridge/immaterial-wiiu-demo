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

#include "scenes/SceneBase.h"
#include "scenes/SceneAssets.h"
#include "renderer/Renderer.h"
#include <whb/log_udp.h>
#include "sound/Music.h"
#include "sync/Sync.h"

#include "renderer/CafeGLSLCompiler.h"
#include "renderer/ShaderManager.h"

int main(int argc, char **argv)
{
   // wiiu homebrew bug workaround (which unfortunately doesn't seem to actually work)
   fprintf(stdout, "hi\n");
   fprintf(stderr, "hello\n");
   
   WHBLogCafeInit();
   WHBLogUdpInit();
   WHBProcInit();
   WHBGfxInit();
   GLSL_Init();
   WHBMountSdCard();

   // Preheat shader cache
   getShaderManager();

   // Load all assets
   getSceneAssets();

   // Current scene
   int currentScene = -1000;
   SceneBase* scene = nullptr;

   // Scene ID -> Scene mapping array
   WHBLogPrint("Hello World! Logging initialised.");
   {
      MusicPlayer music("assets/dumdumdiday.ogg");
      Renderer renderer;
      
      WHBLogPrintf("Begin updating...");
#ifdef SYNC_PLAYER
      music.play();
#endif      
      Sync sync("sync_tracks/", SYNC_IP, &music, 0.1f);
      while (WHBProcIsRunning()) {
         // Update rocket
         sync.update();
         
         // Scene switcher
         int newScene = syncVal("Global:Scene");
         if(currentScene != newScene) {
            if (scene != nullptr) {
               scene->teardown();
               delete scene;
            }
            scene = getScene(newScene);
            scene->setup();
         }

         // Update scene
         scene->update(music.currentTime());
         renderer.renderFrame(*scene);
         
         //WHBLogPrintf("Frame done, playback time is %f", music.currentTime());
      }
   }
   WHBLogPrintf("Done. Quitting...");

   // by moving glsl init to main and not calling glsl shutdown, we apparently dodge a crash on exit? what?
   //GLSL_Shutdown();

   WHBGfxShutdown();
   AXQuit();
   WHBUnmountSdCard();
   WHBLogCafeDeinit();
   WHBProcShutdown();
   WHBLogUdpDeinit();

   return 0;
}
