#include "thread_scratch.h"
#include "arena.h"
#include "error.h"
#include <threads.h>

thread_local Arena g_scratch_arena;

int ThreadScratchArenaInitialise()
{
    void* base_ptr = (void*)Gigabytes(128);
    g_scratch_arena = ArenaInitialise(base_ptr, Megabytes(16), Megabytes(512));
    return CU_SUCCESS;
}

Arena* GetScratchArena()
{
    return &g_scratch_arena;
}

void ThreadScratchArenaShutdown()
{
    ArenaShutdown(&g_scratch_arena);
}