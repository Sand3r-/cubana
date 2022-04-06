#include "stackallocator.h"
#include "log/log.h"
#include "errorcodes.h"
#include <SDL_stdinc.h>
#include <memory.h>
#include <assert.h>

#define TRACABLE_ALLOCS_NUM 1024

struct
{
    void* buffer;
    void* top;
    u64 capacity;
    b8 debug;

    // Statistics and debug
    u16 allocations_num;
    const char* allocation_names[TRACABLE_ALLOCS_NUM];
    void* allocation_ptrs[TRACABLE_ALLOCS_NUM];

} A;

int StackAllocatorInit(b8 debug)
{
    A.capacity = Megabytes(1);
    A.buffer = SDL_malloc(A.capacity);
    A.top = A.buffer;
    A.debug = debug;

    // Fill whole buffer with 0xCC so that it is known that the memory is uninitialised
    if (debug)
    {
        memset(A.buffer, 0xCC, A.capacity);
        StackAllocatorNamedAlloc(0, "Buffer start");
    }

    L_INFO("Stack allocator initialized with %llu Megabytes", A.capacity / 1024 / 1024);
    return CU_SUCCESS;
}

void* StackAllocatorAlloc(u64 size)
{
    if ((u64)A.buffer + A.capacity < (u64)A.top + size)
    {
        u64 excess = (u64)A.top + size - (u64)A.buffer - A.capacity;
        L_ERROR("Out-of-memory. Please increase the total capacity by at least %llu", excess);
    }

    void* ptr = A.top;
    A.top = (u8*)A.top + size;
    return ptr;
}

void* StackAllocatorNamedAlloc(u64 size, const char* name)
{
    if ((u64)A.buffer + A.capacity < (u64)A.top + size)
    {
        u64 excess = (u64)A.top + size - (u64)A.buffer - A.capacity;
        L_ERROR("Out-of-memory. Please increase the total capacity by at least %llu", excess);
    }

    void* ptr = A.top;
    A.top = (u8*)A.top + size;

    if (A.debug)
    {
        // Increase number of tracable allocs if this assert is triggered
        assert(A.allocations_num + 1 < TRACABLE_ALLOCS_NUM);
        A.allocation_ptrs[A.allocations_num] = ptr;
        A.allocation_names[A.allocations_num++] = name;
    }

    return ptr;
}

void StackAllocatorFree(void* ptr)
{
    if (ptr < A.top)
    {
        A.top = ptr;
    }
    else
    {
        L_ERROR("Cannot deallocate an already freed pointer");
        assert(0);
    }
    if (A.debug)
    {
        u64 dealloc_size = (u8*)A.top - (u8*)ptr;
        memset(ptr, 0xCC, dealloc_size);
        for (i32 i = A.allocations_num - 1; i >= 0; i--)
        {
            A.allocations_num--;
            if (A.allocation_ptrs[i] == ptr)
                break;
        }
    }
}

int StackAllocatorShutdown()
{
    SDL_free(A.buffer);
    return CU_SUCCESS;
}

void StackAllocatorSummary()
{
    L_INFO("Total stack allocator summary");
    StackAllocatorHead(A.allocations_num - 1);
}

void StackAllocatorHead(i32 num)
{
    L_INFO("Listing last %u allocations:", num);
    L_INFO("Address\t\tSize\tName", num);
    u64 size = (u64)A.top - (u64)A.allocation_ptrs[A.allocations_num - 1];
    for (i32 i = A.allocations_num - 1; i >= A.allocations_num - num; i--)
    {
        L_INFO("%X\t\t%u\t%s", (u64)A.allocation_ptrs[i], size, A.allocation_names[i]);
        size = (u64)A.allocation_ptrs[i] - (u64)A.allocation_ptrs[i -1];
    }
}

