#include "linearallocator.h"
#include "error.h"
#include "log/log.h"
#include <SDL_stdinc.h>
#include <assert.h>
#include <memory.h>

static struct
{
    void* buffer;
    void* head;
    u64 capacity;
    b8 debug;

    // Statistics and debug
    u16 allocations_num;
} A;

int LinearAllocatorInit(b8 debug)
{
    A.capacity = Megabytes(1);
    A.buffer = SDL_malloc(A.capacity);
    A.debug = debug;
    LinearAllocatorClear();

    L_INFO("Linear allocator initialized with %llu Megabytes", A.capacity / 1024 / 1024);
    return CU_SUCCESS;
}

void LinearAllocatorClear()
{
    A.head = A.buffer;
    // Fill whole buffer with 0xCC so that it is known that the memory is uninitialised
    if (A.debug)
    {
        memset(A.buffer, 0xCC, A.capacity);
        A.allocations_num = 0;
    }
}

int LinearAllocatorShutdown()
{
    SDL_free(A.buffer);
    return CU_SUCCESS;
}

void* LinearAllocatorAlloc(u64 size)
{
    if ((u64)A.buffer + A.capacity < (u64)A.head + size)
    {
        u64 excess = (u64)A.head + size - (u64)A.buffer - A.capacity;
        ERROR("Out-of-memory. Please increase the total capacity by at least %llu", excess);
    }

    if (A.debug)
        A.allocations_num++;

    void* ptr = A.head;
    A.head = (u8*)A.head + size;
    return ptr;
}

