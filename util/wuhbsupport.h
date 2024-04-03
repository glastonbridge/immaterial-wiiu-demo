#include <stdio.h>

#if WUHB_BUILD

// A WUHB build is always a player mode build
#define SYNC_PLAYER 1

// A WUHB build does not contain the shader compiler
#define CACHE_ONLY 1

// A WUHB does not use emulator fixes
#undef EMU_KLUDGES
#undef UFBX_TRACE
#undef BENCHMARK

// Path prefix for for romfs
#define WIIU_PATH_PREFIX "/vol/content/"

// Who needs log output, anyways
#define WHBLogPrintf(...)

#else

#define WIIU_PATH_PREFIX ""

#endif
