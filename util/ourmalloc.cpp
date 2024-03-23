#include <coreinit/memexpheap.h>
#include <malloc.h>
#include <string.h>
#include <whb/log.h>

#undef DEBUG_OURMALLOC

#ifndef DEBUG_OURMALLOC
#define WHBLogPrintf(...)
#endif

extern MEMHeapHandle ourHeap; // custom heap so that the glsl compiler cannot fuck us up

#ifdef __cplusplus
extern "C" {
#endif
    // Redefine malloc family of functions in terms of our heap
    void* __our_malloc( size_t size ) {
        WHBLogPrintf("Malloc of size %d", size);
        void* ret_addr = MEMAllocFromExpHeapEx(ourHeap, size, 4);
        WHBLogPrintf("Allocated, zeroing: %p", ret_addr);
        memset(ret_addr, 0, size);
        WHBLogPrintf("Malloc complete");
        return ret_addr;
    }

    void* __our_memalign(size_t alignment, size_t size) {
        if(alignment < 4) {
            alignment = 4;
        }
        WHBLogPrintf("Memalign of size %d alignment %d", size, alignment);
        void* ret_addr = MEMAllocFromExpHeapEx(ourHeap, size, alignment);
        WHBLogPrintf("Allocated, zeroing: %p", ret_addr);
        memset(ret_addr, 0, size);
        WHBLogPrintf("Memalign complete");
        return ret_addr;
    }

    void* __our_calloc( size_t num, size_t size ) {
        WHBLogPrintf("Calloc of size %d count %d", size, num);
        void* ret_addr = __our_malloc(size * num);
        return ret_addr;
    }

    void __our_free( void *ptr ) {
        WHBLogPrintf("Free: %p", ptr);
        MEMFreeToExpHeap(ourHeap, ptr);
        WHBLogPrintf("Free complete");
    }

    void* __our_realloc( void *ptr, size_t new_size ) {
        // really poor realloc lol this should 100% be done differently
        // doesn't respect alignment either (does realloc usually? idk)
        WHBLogPrintf("Realloc of %p to new size %d", ptr, new_size);
        void* new_ptr = __our_malloc(new_size);
        if(ptr != NULL) {
            WHBLogPrintf("Memcpy in progress");
            memcpy(new_ptr, ptr, new_size);
            __our_free(ptr);
        }
        return new_ptr;
    }
#ifdef __cplusplus
}
#endif