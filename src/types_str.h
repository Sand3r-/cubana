#pragma once
#include "memory/arena.h"

// Freeing the memory is caller's responsibility
char* CStrFromMemSizeA(Arena* arena, u64 size);
// Reserve at least 10 bytes for out
void  CStrFromMemSize(char* out, u64 out_size, u64 size);
