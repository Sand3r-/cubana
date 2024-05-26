#pragma once
#include "types.h"

#ifdef _WIN32
#define MAX_PATH 260
#else
#define MAX_PATH 4096
#endif

b8 ProcessPlatformEvents(void);