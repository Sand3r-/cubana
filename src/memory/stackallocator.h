#pragma once
#include "types.h"

#ifdef _DEBUG
#define StackMalloc(size, name) StackAllocatorNamedAlloc(size, name)
#else
#define StackMalloc(size, name) StackAllocatorAlloc(size);
#endif

#define StackFree(ptr) StackAllocatorFree(ptr)

int StackAllocatorInit(b8 debug);
int StackAllocatorShutdown();
void* StackAllocatorAlloc(u64 size);
void* StackAllocatorNamedAlloc(u64 size, const char* name);
void StackAllocatorFree(void* ptr);
void StackAllocatorSummary();
void StackAllocatorHead(i32 num);
