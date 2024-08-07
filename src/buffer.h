#pragma once
#include "memory/arena.h"

typedef struct Buffer
{
    void* ptr;
    size_t length;
} Buffer;

Buffer BufferFromFile(Arena* arena, const char* filename);
const char* CStringFromBuffer(Buffer* buffer);