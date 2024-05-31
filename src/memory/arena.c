#include "arena.h"
#include "error.h"
#include "math/scalar.h"
#include "os/os_memory.h"
#include "os/os_error.h"
#include "types_str.h"

#define AlignPow2(x,b) (((x) + (b) - 1)&(~((b) - 1)))

static u64 g_commit_size = 0;
static void InitArenaDebugInfo(Arena* arena);

Arena ArenaInitialise(void* base_ptr, u64 size, u64 capacity)
{
    u64 page_size = cu_os_get_page_size();
    g_commit_size = page_size;
    Arena arena = {
        .pos = 0,
        .capacity = AlignPow2(capacity, page_size),
        .size = AlignPow2(size, page_size)
    };
    // Reserve some capacity in the Virtual Address space, but don't commit it
    arena.base = cu_os_reserve(base_ptr, arena.capacity);

    // Commit only some part of the reserved memory
    if(!cu_os_commit(arena.base, arena.size))
    {
        cu_os_print_last_error();
        cu_os_release(arena.base, arena.capacity);
        arena.base = NULL;
        ERROR("Failed to commit memory during arena initialization");
    }

    char size_str[16], capacity_str[16];
    CStrFromMemSize(size_str, 16, arena.size);
    CStrFromMemSize(capacity_str, 16, arena.capacity);
    L_INFO("Initialised arena of %s commited and %s reserved", size_str, capacity_str);

#ifdef DEBUG_ARENA_ALLOCATIONS
    InitArenaDebugInfo(&arena);
#endif

    return arena;
}

void ArenaShutdown(Arena* arena)
{
    cu_os_release(arena->base, arena->capacity);
}

void* ArenaPushNoZero(Arena* arena, u64 size)
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

    return ptr;
}


void* ArenaPush(Arena* arena, u64 size)
{
    u8* beg = (u8*)arena->base + arena->pos;
    void* ptr = ArenaPushNoZero(arena, size);
    memset(beg, 0, size);
    return ptr;
}

static void UpdateDebugAllocations(Arena* arena);

void ArenaPopTo(Arena* arena, u64 pos)
{
    assert(pos <= arena->pos);
    arena->pos = pos;

    #ifdef DEBUG_ARENA_ALLOCATIONS
    UpdateDebugAllocations(arena);
    #endif
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

ArenaMarker ArenaMarkerCreate(Arena* arena)
{
    ArenaMarker marker = {arena, arena->pos};
    return marker;
}

void ArenaMarkerRollback(ArenaMarker marker)
{
    ArenaPopTo(marker.arena, marker.pos);
}


#ifdef DEBUG_ARENA_ALLOCATIONS

static void InitArenaDebugInfo(Arena* arena)
{
    arena->debug_info = ArenaPush(arena, sizeof(ArenaDebugInfo));
    arena->debug_info->print_new_allocations = true;
    arena->debug_info->allocations_num = 1;
    arena->debug_info->allocation_ptrs[0] = arena->debug_info;
    cu_snprintf(
        arena->debug_info->allocation_names[0], MAX_ALLOC_NAME, "Arena debug info");
    L_DEBUG("New alloc: %s:%d - %llu", "Arena debug info", 1337, sizeof(ArenaDebugInfo));
}

static void UpdateDebugAllocations(Arena* arena)
{
    ArenaDebugInfo* debug = arena->debug_info;
    do
    {
        u8* current_ptr = (u8*)arena->base + arena->pos;
        u8* alloc_ptr = debug->allocation_ptrs[debug->allocations_num - 1];
        if (current_ptr <= alloc_ptr)
            debug->allocations_num--;
        else
            break;
    } while (debug->allocations_num);
}

void* ArenaPushDebug(Arena* arena, u64 size, const char* file, int line)
{
    u8* beg = (u8*)arena->base + arena->pos;
    void* ptr = ArenaPushNoZeroDebug(arena, size, file, line);
    memset(beg, 0, size);
    return ptr;
}

void* ArenaPushNoZeroDebug(Arena* arena, u64 size, const char* file, int line)
{
    void* ptr = ArenaPushNoZero(arena, size);

    ArenaDebugInfo* debug = arena->debug_info;
    assert(debug->allocations_num < TRACABLE_ALLOCS_NUM);

    cu_snprintf(debug->allocation_names[debug->allocations_num],
        MAX_ALLOC_NAME, "%s:%d", file, line);
    debug->allocation_ptrs[debug->allocations_num] = ptr;
    debug->allocations_num++;

    if (arena->debug_info->print_new_allocations)
         L_DEBUG("New alloc: %s:%d - %llu", file, line, size);

    return ptr;
}

void DEBUG_SetPrintNewAllocations(Arena* arena, b8 value)
{
    arena->debug_info->print_new_allocations = value;
}

void DEBUG_ArenaPrintAllocations(Arena* arena)
{
    L_DEBUG("Total allocations: %lu", arena->debug_info->allocations_num);
    L_DEBUG("Total size occupied: %llu", arena->pos);
    L_DEBUG("Total size commited: %llu", arena->size);
    L_DEBUG("Total size reserved: %llu", arena->capacity);
    L_DEBUG("Address\t\tSize (bytes)\tName");
    for (u32 i = 0; i < arena->debug_info->allocations_num - 1; i++)
    {
        const char* name = arena->debug_info->allocation_names[i];
        void* ptr = arena->debug_info->allocation_ptrs[i];
        void* size = (void*)((u8*)arena->debug_info->allocation_ptrs[i + 1] -
                   (u8*)arena->debug_info->allocation_ptrs[i]);
        L_DEBUG("0x%" PRIxPTR "\t%llu\t\t%s", ptr, size, name);
    }
    // Handle tail differently due to different size computation
    u32 last_index = arena->debug_info->allocations_num - 1;
    const char* name = arena->debug_info->allocation_names[last_index];
    void* ptr = arena->debug_info->allocation_ptrs[last_index];
    void* size = (void*)((((u8*)arena->base + arena->pos))
               - (u8*)arena->debug_info->allocation_ptrs[last_index]);
    L_DEBUG("0x%" PRIxPTR "\t%llu\t\t%s", ptr, size, name);
}
#endif