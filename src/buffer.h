#pragma once

typedef struct Buffer
{
    void* ptr;
    size_t length;
} Buffer;

Buffer AllocBuffer(size_t size, const char* alloc_name);
Buffer FreeBuffer(Buffer buffer);
Buffer File2Buffer(const char* filename);