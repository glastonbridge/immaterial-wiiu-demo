#pragma once

// #define USE_OURMALLOC
#undef USE_OURMALLOC
#undef DEBUG_OURMALLOC

#include <stddef.h>

#ifdef USE_OURMALLOC

#ifdef __cplusplus
extern "C" {
#endif
    void* __our_malloc( size_t size );
    void* __our_memalign(size_t alignment, size_t size);
    void* __our_calloc( size_t num, size_t size );
    void __our_free( void *ptr );
    void* __our_realloc( void *ptr, size_t new_size );
#ifdef __cplusplus
}
#endif

#define malloc(size) __our_malloc(size)
#define memalign(alignment, size) __our_memalign(alignment, size)
#define calloc(num, size) __our_calloc(num, size)
#define free(ptr) __our_free(ptr)
#define realloc(ptr, new_size) __our_realloc(ptr, new_size)

#else

#include <malloc.h>

#endif
