#include <cstdint>

#define SWAP_32(x)                                                             \
  ((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) |    \
   (((x) << 24) & 0xff000000))

void swap_memcpy(void *dst, const void *src, uint32_t size);