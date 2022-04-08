#pragma once
#include "types.h"

#define LinearMalloc(size) LinearAllocatorAlloc(size)

int LinearAllocatorInit(b8 debug);
void LinearAllocatorClear();
int LinearAllocatorShutdown();
void* LinearAllocatorAlloc(u64 size);