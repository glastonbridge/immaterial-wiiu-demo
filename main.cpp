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
#include <coreinit/memblockheap.h>
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
#ifdef USE_OURMALLOC_BLOCKHEAP
MEMHeapHandle ourHeap;
MEMBlockHeap ourHeapStorage;
#else
MEMHeapHandle ourHeap;
#endif
#endif

#define BENCHMARK 1

#include "util/wuhbsupport.h"

int main(int argc, char **argv)
{
   // wiiu homebrew bug workaround (which unfortunately doesn't seem to actually work)
   fprintf(stdout, "hi\n");
   fprintf(stderr, "hello\n");

#ifndef WUHB_BUILD
   WHBLogCafeInit();
   WHBLogUdpInit();
#endif
#ifndef SHADER_BUILD
   WHBProcInit();
#endif
   WHBGfxInit();
#ifndef WUHB_BUILD
   WHBMountSdCard();
#endif

   // Preheat shader cache
   getShaderManager();

#if SHADER_BUILD
   // if we're in "build shader cache" mode, quit here
   WHBGfxShutdown();
   WHBUnmountSdCard();
   WHBLogCafeDeinit();
   WHBLogUdpDeinit();
   return 0;
#endif

   // Init the heap, 750mb, more than enough to store all assets
   // this is explicitly and intentionally NOT used as the default heap, to isolate us from the base
   // wiiu memory management that WHB uses, because it gets fucked up somehow at some point (maybe due to
   // glsl compiler)
   uint32_t heapSize = int(1024*1024*1025*0.75);
   uint8_t* heapBaseAddr = (uint8_t*)MEMAllocFromDefaultHeapEx(heapSize, 4);
   WHBLogPrintf("Allocated heap at %p", heapBaseAddr);
#ifdef USE_OURMALLOC_BLOCKHEAP
   size_t trackSize = heapSize/1024;
   uint8_t* heapEndAddr = heapBaseAddr+heapSize;
   MEMBlockHeapTracking* blockTrack = (MEMBlockHeapTracking*)MEMAllocFromDefaultHeapEx(trackSize, 4);
   WHBLogPrintf("Block track at %p, end at %p", blockTrack, heapEndAddr);
   ourHeap = MEMInitBlockHeap(&ourHeapStorage, heapBaseAddr, heapBaseAddr+heapSize, blockTrack, trackSize, 0);
#else
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
   music->play();
#endif
   createSyncHandler(
      "sync_tracks/", 
      SYNC_IP, 
      music, 
      (60.0f / 100.0f) / 6.0f // 100 BPM, 8 rows per beat. unsure if FP math would cause drift by being not 100% accurate, should be fine tho
   );
   size_t frameCounter = 0;
   float lastTime = 0.0f;
   while (WHBProcIsRunning()) {
      // Update rocket
      getSyncHandler()->update();
      
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

#ifdef BENCHMARK
      // if you're in synctool mode you can make the FPS go negative lol
      frameCounter++;
      if(frameCounter % 60 == 0) {
         float currentTime = music->currentTime();
         WHBLogPrintf("FPS: %f", 60.0f / (currentTime - lastTime));
         lastTime = currentTime;
      }
#endif
   }
   WHBLogPrintf("Done. Quitting...");

   // Get rid of all of our stuff
   destroySyncHandler();
   delete renderer;
   delete music;
   if (scene != nullptr) {
      scene->teardown();
      delete scene;
   }
   destroySceneAssets();
   destroyShaderManager();

   // Clean up GX2 / AX stuff
   WHBGfxShutdown();
   AXQuit();

#ifndef WUHB_BUILD
   // If not in WUHB mode: Deinit logging, unmount SD
   WHBUnmountSdCard();
   WHBLogCafeDeinit();
#endif

   // Deinit WHBProc
   WHBProcShutdown();

#ifndef WUHB_BUILD
   // Deinit UDP log
   WHBLogPrintf("Deinitialising UDP logging...");
   WHBLogUdpDeinit();
#endif

#ifdef USE_OURMALLOC
   // Kill custom heap, if we were using that
   MEMDestroyExpHeap(ourHeap);
   MEMFreeToDefaultHeap(heapBaseAddr);
#endif
   return 0;
}
