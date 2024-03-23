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
#include <coreinit/memexpheap.h>
#include <sndcore2/core.h>

#include "scenes/SceneBase.h"
#include "scenes/SceneAssets.h"
#include "renderer/Renderer.h"
#include <whb/log_udp.h>
#include "sound/Music.h"
#include "sync/Sync.h"

#include "renderer/CafeGLSLCompiler.h"
#include "renderer/ShaderManager.h"

// Non-default heap
MEMHeapHandle ourHeap;

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

   // Init the heap, 750mb
   // this is explicitly and intentionally NOT used as the default heap, to isolate us from the base
   // wiiu memory management that WHB uses, because it gets fucked up somehow by the GLSL compiler
   WHBLogCafeInit();
   WHBLogUdpInit();
   uint32_t heapSize = int(1024*1024*1025*0.75);
   void* heapBaseAddr = MEMAllocFromDefaultHeapEx(heapSize, 4);
   WHBLogPrintf("Allocated heap at %p", heapBaseAddr);
   ourHeap = MEMCreateExpHeapEx(heapBaseAddr, heapSize, 0);

   // Load all assets
   getSceneAssets();

   // Current scene
   int currentScene = -1000;
   SceneBase* scene = nullptr;

   // Scene ID -> Scene mapping array
   WHBLogPrint("Hello World! Logging initialised.");
   {
      MusicPlayer music("assets/clairvoyance.ogg", 8.1f);
      Renderer renderer;
      
      WHBLogPrintf("Begin updating...");
#ifdef SYNC_PLAYER
      music.play();
#endif      
      Sync sync(
         "sync_tracks/", 
         SYNC_IP, 
         &music, 
         (60.0f / 107.0f) / 8.0f // 107 BPM, 4 rows per beat. unsure if FP math would cause drift by being not 100% accurate, should be fine tho
      );
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
            currentScene = newScene;
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
   MEMDestroyExpHeap(ourHeap);
   MEMFreeToDefaultHeap(heapBaseAddr);
   WHBGfxShutdown();
   AXQuit();
   WHBUnmountSdCard();
   WHBLogCafeDeinit();
   WHBProcShutdown();
   WHBLogUdpDeinit();

   return 0;
}
