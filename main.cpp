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

#include "renderer/ShaderManager.h"

#include "util/ourmalloc.h"

// Non-default heap
#ifdef USE_OURMALLOC
MEMHeapHandle ourHeap;
#endif

int main(int argc, char **argv)
{
   // wiiu homebrew bug workaround (which unfortunately doesn't seem to actually work)
   fprintf(stdout, "hi\n");
   fprintf(stderr, "hello\n");
   
   WHBLogCafeInit();
   WHBLogUdpInit();
   WHBProcInit();
   WHBGfxInit();
   WHBMountSdCard();

   // Preheat shader cache
   getShaderManager();

   // Init the heap, 750mb
   // this is explicitly and intentionally NOT used as the default heap, to isolate us from the base
   // wiiu memory management that WHB uses, because it gets fucked up somehow by the GLSL compiler
#ifdef USE_OURMALLOC
   uint32_t heapSize = int(1024*1024*1025*0.75);
   void* heapBaseAddr = MEMAllocFromDefaultHeapEx(heapSize, 4);
   WHBLogPrintf("Allocated heap at %p", heapBaseAddr);
   ourHeap = MEMCreateExpHeapEx(heapBaseAddr, heapSize, 0);
#endif

   // Load all assets
   getSceneAssets();

   // Current scene
   int currentScene = -1000;
   SceneBase* scene = nullptr;
   WHBLogPrint("Hello World! Logging initialised.");
   MusicPlayer* music = new MusicPlayer("assets/immaterial.ogg", 0.0f);
   Renderer* renderer = new Renderer();
   
   WHBLogPrintf("Begin updating...");
#ifdef SYNC_PLAYER
   music.play();
#endif      
   Sync* sync = new Sync(
      "sync_tracks/", 
      SYNC_IP, 
      music, 
      (60.0f / 100.0f) / 6.0f // 100 BPM, 8 rows per beat. unsure if FP math would cause drift by being not 100% accurate, should be fine tho
   );
   while (WHBProcIsRunning()) {
      // Update rocket
      sync->update();
      
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
      scene->update(music->currentTime());
      renderer->renderFrame(*scene);
      
      //WHBLogPrintf("Frame done, playback time is %f", music.currentTime());
   }
   WHBLogPrintf("Done. Quitting...");

   // by not calling glsl shutdown, we apparently dodge a crash on exit, sometimes. but also now we have shader caching 
   // and in most cases we don't need to recompile shaders so there won't be a need to load the GLSL compiler anyways
   //GLSL_Shutdown();

   // Other things we're not doing: properly deleting... everything
   // TODO: figure out why cleanup crashes

   WHBGfxShutdown();
   AXQuit();
   WHBUnmountSdCard();
   WHBLogCafeDeinit();
   WHBProcShutdown();
   WHBLogPrintf("Deinitialising UDP logging...");
   WHBLogUdpDeinit();


#ifdef USE_OURMALLOC
   // We should, technically, destroy the heap here, but it seems to cause a crash on exit as well so we'll just pray that 
   // the OS reclaims all the memory we allocated
   //MEMDestroyExpHeap(ourHeap);
   //MEMFreeToDefaultHeap(heapBaseAddr);
#endif
   return 0;
}
