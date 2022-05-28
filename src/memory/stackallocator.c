#include "stackallocator.h"
#include "log/log.h"
#include "error.h"
#include <SDL_stdinc.h>
#include <SDL_thread.h>
#include <SDL_endian.h>
#include <assert.h>
#include <memory.h>
#include <signal.h>

#define TRACABLE_ALLOCS_NUM 1024
#define UNALLOC_BYTE 0xCC

static struct
{
    void*       buffer;
    void*       top;
    u64         capacity;
    b8          debug;

    // Statistics and debug
    i16         allocations_num;
    const char* allocation_names[TRACABLE_ALLOCS_NUM];
    void*       allocation_ptrs[TRACABLE_ALLOCS_NUM];
    u32         separator;

    // Watchdog thread
    SDL_Thread* watchdog_thread;
    b8          stop_watchdog;
} A;

static int Watchdog(void* dummy);

int StackAllocatorInit(b8 debug)
{
    A.capacity = Megabytes(1);
    A.buffer = SDL_malloc(A.capacity);
    A.top = A.buffer;
    A.debug = debug;


    if (debug)
    {
        A.separator = SDL_SwapBE32(0x00FACADE); // Used as a preamble to every allocation
        // Fill whole buffer with magic number so that it is known that the memory is uninitialised
        memset(A.buffer, UNALLOC_BYTE, A.capacity);
        StackAllocatorNamedAlloc(0, "Buffer start");

        // Launch watchdog
        A.stop_watchdog = false;
        A.watchdog_thread = SDL_CreateThread(Watchdog, "Watchdog", (void *)NULL);
        if (A.watchdog_thread == NULL)
        {
            ERROR("Watchdog thread creation failed: %s", SDL_GetError());
        }
    }

    L_INFO("Stack allocator initialized with %llu Megabytes", A.capacity / 1024 / 1024);
    return CU_SUCCESS;
}

void* StackAllocatorAlloc(u64 size)
{
    if (A.debug)
        size += sizeof(A.separator);

    if ((u64)A.buffer + A.capacity < (u64)A.top + size)
    {
        u64 excess = (u64)A.top + size - (u64)A.buffer - A.capacity;
        ERROR("Out-of-memory. Please increase the total capacity by at least %llu", excess);
    }

    void* ptr = A.top;
    A.top = (u8*)A.top + size;

    if (A.debug)
    {
        A.allocations_num++;
        *((u32*)ptr)++ = A.separator; // Prepend allocation with a separator
    }

    return ptr;
}

void* StackAllocatorNamedAlloc(u64 size, const char* name)
{
    if (A.debug)
        size += sizeof(A.separator);

    if ((u64)A.buffer + A.capacity < (u64)A.top + size)
    {
        u64 excess = (u64)A.top + size - (u64)A.buffer - A.capacity;
        ERROR("Out-of-memory. Please increase the total capacity by at least %llu", excess);
    }

    void* ptr = A.top;
    A.top = (u8*)A.top + size;

    if (A.debug)
    {
        *((u32*)ptr)++ = A.separator; // Prepend allocation with a separator
        // Increase number of tracable allocs if this assert is triggered
        assert(A.allocations_num + 1 < TRACABLE_ALLOCS_NUM);
        A.allocation_ptrs[A.allocations_num] = ptr;
        A.allocation_names[A.allocations_num++] = name;
    }

    return ptr;
}

void StackAllocatorFree(void* ptr)
{
    if (A.debug)
    {
        u64 dealloc_size = (u8*)A.top - (u8*)ptr + sizeof(A.separator);
        for (i32 i = A.allocations_num - 1; i >= 0; i--)
        {
            A.allocations_num--;
            if (A.allocation_ptrs[i] == ptr)
                break;
        }
        *((u32*)ptr)--; // Move the ptr to point at the separator before freeing

        // Mark freed memory
        memset((u8*)ptr, UNALLOC_BYTE, dealloc_size);
    }

    if (ptr < A.top)
    {
        A.top = ptr;
    }
    else
    {
        ERROR("Cannot deallocate an already freed pointer");
    }
}

int StackAllocatorShutdown(void)
{
    DEBUG_StopWatchdog();
    SDL_free(A.buffer);
    return CU_SUCCESS;
}

void StackAllocatorSummary(void)
{
    L_INFO("Total stack allocator summary");
    i32 allocationsToList = A.allocations_num - 1;
    StackAllocatorHead(allocationsToList < 0 ? 0 : allocationsToList);
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

/*
    A routine continuously checking for any memory corruption.
    It does 2 things:
    1. Traverses through allocation done, checks if every allocation is
        preceeded by a 0x00FACADE magic number
    2. Traversers through unallocated region of memory to see if all bytes are
        set to UNALLOC_BYTE.
*/
static int Watchdog(void* dummy)
{
    L_DEBUG("Watchdog started");
    while(!A.stop_watchdog)
    {
        u8* top = (u8*)A.top;
        u8* end = (u8*)A.buffer + A.capacity;

        for (i32 i = 0; i < A.allocations_num; i++)
        {
            if (*(((u32*)A.allocation_ptrs[i]) - 1) != A.separator)
            {
                ERROR("Memory corruption detected overwriting allocation %s", A.allocation_names[i]);
                StackAllocatorSummary();
            }
        }

        for (u8* head = top; head < end; head++)
        {
            if (*head != UNALLOC_BYTE)
            {
                ERROR("Memory corruption detected at adress %p", head);
                StackAllocatorSummary();
            }
        }
    }

    return CU_SUCCESS;
}

void DEBUG_StopWatchdog(void)
{
    if (A.watchdog_thread != NULL && !A.stop_watchdog)
    {
        SDL_Delay(1000); // Give Watchdog some time to detect possible failrues
        A.stop_watchdog = true;
        SDL_WaitThread(A.watchdog_thread, NULL);
        L_DEBUG("Watchdog stopped");
    }
}