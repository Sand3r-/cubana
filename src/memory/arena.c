#include "arena.h"
#include "error.h"
#include "math/scalar.h"

#define AlignPow2(x,b) (((x) + (b) - 1)&(~((b) - 1)))

static u64 g_commit_size = 0;

Arena ArenaInitialise(u64 size, u64 capacity)
{
    u64 page_size = cu_os_get_page_size();
    g_commit_size = page_size;
    Arena arena = {
        .pos = 0,
        .capacity = AlignPow2(capacity, page_size),
        .size = AlignPow2(size, page_size)
    }; 
    // Reserve some capacity in the Virtual Address space, but don't commit it
    arena.base = cu_os_reserve(arena.capacity);

    // Commit only some part of the reserved memory
    if(!cu_os_commit(arena.base, size))
    {
        cu_os_release(arena.base, arena.capacity);
        arena.base = NULL;
        ERROR("Failed to commit memory during arena initialization");
    }
    
    L_INFO("Initialised arena allocator of %llu KB commited and %llu KB reserved",
           arena.capacity / 1024, arena.size / 1024);
    return arena;
}

void ArenaShutdown(Arena* arena)
{
    cu_os_release(arena->base, arena->capacity);
}

#ifndef DEBUG_ARENA_ALLOCATIONS
void* ArenaPushNoZero(Arena* arena, u64 size)
#else
void* ArenaPushNoZero(Arena* arena, u64 size, const char* file, int line)
#endif
{
    u64 new_pos = arena->pos + size;
    assert(new_pos < arena->capacity);

    // If the allocation exceeds the current size, commit more memory
    if (new_pos > arena->size)
    {
        // Round up to the nearest multiple of a commit size
        u64 aligned_pos = AlignPow2(new_pos, g_commit_size);
        // Clamp to the total capacity of the arena
        u64 clamped_pos = min(aligned_pos, arena->capacity);
        // Compute the excess memory to be committed
        u64 excess = clamped_pos - arena->size;
        if (cu_os_commit((u8*)arena->base + arena->size, excess))
            // Update the arena size to its new length
            arena->size = clamped_pos;
        else
            ERROR("Out-of-memory. Please increase the total capacity by"
                  "at least %llu", excess);
    }

    void* ptr = (u8*)arena->base + arena->pos;
    arena->pos = new_pos; // Move position pointer

#ifdef DEBUG_ARENA_ALLOCATIONS
    assert(arena->allocations_num < TRACABLE_ALLOCS_NUM);
    arena->allocation_ptrs[arena->allocations_num] = ptr;
    cu_snprintf(arena->allocation_names[arena->allocations_num],
        MAX_ALLOC_LOC_NAME, "%s:%d", file, line);
    arena->allocations_num++;
#endif

    return ptr;
}

#ifndef DEBUG_ARENA_ALLOCATIONS
void* ArenaPush(Arena* arena, u64 size)
{
    u8* beg = (u8*)arena->base + arena->pos;
    void* ptr = ArenaPushNoZero(arena, size);
    memset(beg, 0, size);
    return ptr;
}
#else
void* ArenaPush(Arena* arena, u64 size, const char* file, int line)
{
    u8* beg = (u8*)arena->base + arena->pos;
    void* ptr = ArenaPushNoZero(arena, size, file, line);
    memset(beg, 0, size);
    return ptr;
}
#endif


void ArenaPopTo(Arena* arena, u64 pos)
{
    assert(pos <= arena->pos);
    arena->pos = pos;
}

void ArenaPop(Arena* arena, u64 size)
{
    assert(arena->pos >= size);
    u64 new_pos = arena->pos - size;
    ArenaPopTo(arena, new_pos);
}

void ArenaReset(Arena* arena)
{
    ArenaPopTo(arena, 0);
}

#ifdef DEBUG_ARENA_ALLOCATIONS
void DEBUG_ArenaPrintAllocations(Arena* arena)
{
    L_INFO("Total allocations: %u", arena->allocations_num);
    L_INFO("Total size occupied: %u", arena->pos);
    L_INFO("Total size commited: %u", arena->size);
    L_INFO("Total size reserved: %u", arena->capacity);
    L_INFO("Allocations:");
    for (u32 i = 0; i < arena->allocations_num; i++)
    {
        L_INFO("%s %p", arena->allocation_names[i], arena->allocation_ptrs[i]);
    }
}
#endif