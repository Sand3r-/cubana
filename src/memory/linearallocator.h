#pragma once
#include "types.h"

#define LinearMalloc(size) LinearAllocatorAlloc(size)

int LinearAllocatorInit(b8 debug);
void LinearAllocatorClear(void);
int LinearAllocatorShutdown(void);
void* LinearAllocatorAlloc(u64 size);