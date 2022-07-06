#pragma once
#include "types.h"

// Return number of milliseconds since program startup
u64 GetTicks(void);

u64 GetPerformanceCounter(void);
u64 GetPerformanceFrequency(void);