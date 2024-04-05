#include "../util/ourmalloc.h"
#include <coreinit/memblockheap.h>
#include <coreinit/memexpheap.h>
#include <malloc.h>
#include <string.h>
#include <whb/log.h>

#ifndef DEBUG_OURMALLOC
#define WHBLogPrintf(...)
#endif

#ifdef USE_OURMALLOC_BLOCKHEAP
#define MEMGetTotalFreeSize MEMGetTotalFreeSizeForBlockHeap
#else
#define MEMGetTotalFreeSize MEMGetTotalFreeSizeForExpHeap
#endif

#ifdef USE_OURMALLOC
extern MEMHeapHandle
    ourHeap; // custom heap so that the glsl compiler cannot fuck us up

#ifdef __cplusplus
extern "C" {
#endif
// Redefine malloc family of functions in terms of our heap
void *__our_malloc(size_t size) {
  WHBLogPrintf("Malloc of size %d", size);
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
#ifdef USE_OURMALLOC_BLOCKHEAP
  void *ret_addr = MEMAllocFromBlockHeapEx(ourHeap, size, 4);
#else
  void *ret_addr = MEMAllocFromExpHeapEx(ourHeap, size, 4);
#endif
  WHBLogPrintf("Allocated, zeroing: %p", ret_addr);
  memset(ret_addr, 0, size);
  WHBLogPrintf("Malloc complete");
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
  return ret_addr;
}

void *__our_memalign(size_t alignment, size_t size) {
  if (alignment < 4) {
    alignment = 4;
  }
  WHBLogPrintf("Memalign of size %d alignment %d", size, alignment);
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
#ifdef USE_OURMALLOC_BLOCKHEAP
  void *ret_addr = MEMAllocFromBlockHeapEx(ourHeap, size, alignment);
#else
  void *ret_addr = MEMAllocFromExpHeapEx(ourHeap, size, alignment);
#endif
  WHBLogPrintf("Allocated, zeroing: %p", ret_addr);
  memset(ret_addr, 0, size);
  WHBLogPrintf("Memalign complete");
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
  return ret_addr;
}

void *__our_calloc(size_t num, size_t size) {
  WHBLogPrintf("Calloc of size %d count %d", size, num);
  void *ret_addr = __our_malloc(size * num);
  return ret_addr;
}

void __our_free(void *ptr) {
  WHBLogPrintf("Free: %p", ptr);
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
#ifdef USE_OURMALLOC_BLOCKHEAP
  MEMFreeToBlockHeap(ourHeap, ptr);
#else
  MEMFreeToExpHeap(ourHeap, ptr);
#endif
  WHBLogPrintf("Free complete");
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
}

void *__our_realloc(void *ptr, size_t new_size) {
  // really poor realloc lol this should 100% be done differently
  // doesn't respect alignment either (does realloc usually? idk)
  WHBLogPrintf("Realloc of %p to new size %d", ptr, new_size);
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
  void *new_ptr = __our_malloc(new_size);
  if (ptr != NULL) {
    WHBLogPrintf("Memcpy in progress");
    memcpy(new_ptr, ptr, new_size);
    __our_free(ptr);
  }
  WHBLogPrintf("Free space now: %d", MEMGetTotalFreeSize(ourHeap));
  return new_ptr;
}
#ifdef __cplusplus
}
#endif
#endif