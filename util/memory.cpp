#include "memory.h"

void swap_memcpy(void *dst, const void *src, uint32_t size) {
  uint32_t *csrc = (uint32_t *)src;
  uint32_t *cdst = (uint32_t *)dst;
  for (uint32_t i = 0; i < (size >> 2); i++) {
    cdst[i] = SWAP_32(csrc[i]);
  }
}