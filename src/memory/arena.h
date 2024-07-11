#pragma once
#include "types.h"

/*
    Arena allocator

    Description: Allocate a memory region per lifetime/context.

    Prefer using Helper macros:
        - PushArrayNoZero
        - PushArray
        - PushStructNoZero
        - PushStruct
    instead of:
        - ArenaPushNoZero
        - ArenaPush

    as the macros ensure that when DEBUG_ARENA_ALLOCATIONS is enabled,
    the ArenaPush and ArenaPushNoZero are replaced with their debug
    equivalents which retain more info.
*/


// Public API
typedef struct ArenaDebugInfo ArenaDebugInfo;

typedef struct Arena {
    void* base;
    u64 pos;
    u64 size;
    u64 capacity;

    #ifdef DEBUG_ARENA_ALLOCATIONS
    ArenaDebugInfo* debug_info;
    #endif

} Arena;

Arena ArenaInitialise(void* base_ptr, u64 size, u64 capacity);
void ArenaShutdown(Arena* arena);

#define PushArrayNoZero(arena, type, count) (type*)ArenaPushNoZero((arena), sizeof(type) * (count))
#define PushArray(arena, type, count)       (type*)ArenaPush((arena), sizeof(type) * (count))
#define PushStructNoZero(arena, type)       PushArrayNoZero((arena), type, 1)
#define PushStruct(arena, type)             PushArray((arena), type, 1)

void ArenaPopTo(Arena* arena, u64 pos);
void ArenaPop(Arena* arena, u64 size);
void ArenaReset(Arena* arena);
void ArenaResize(Arena* arena, u64 size);

// Internal API
void* ArenaPushNoZero(Arena* arena, u64 size);
void* ArenaPush(Arena* arena, u64 size);

typedef struct ArenaMarker
{
    Arena* arena;
    u64    pos;
} ArenaMarker;

ArenaMarker ArenaMarkerCreate(Arena* arena);
void ArenaMarkerRollback(ArenaMarker marker);

#define with_arena(arena) for (                    \
    ArenaMarker marker = ArenaMarkerCreate(arena); \
    marker.pos;                                 \
    ArenaMarkerRollback(marker), marker.pos = 0)

// Debug API
#ifdef DEBUG_ARENA_ALLOCATIONS
void* ArenaPushDebug(Arena* arena, u64 size, const char* file, int line);
void* ArenaPushNoZeroDebug(Arena* arena, u64 size, const char* file, int line);
#undef PushArrayNoZero
#undef PushArray
#define PushArrayNoZero(arena, type, count) (type*)ArenaPushNoZeroDebug((arena), sizeof(type) * (count), FILE_BASENAME, __LINE__)
#define PushArray(arena, type, count)       (type*)ArenaPushDebug((arena), sizeof(type) * (count), FILE_BASENAME, __LINE__)

#define TRACABLE_ALLOCS_NUM 1024
#define MAX_ALLOC_NAME 256

typedef struct ArenaDebugInfo
{
    void* allocation_ptrs[TRACABLE_ALLOCS_NUM];
    char allocation_names[TRACABLE_ALLOCS_NUM][MAX_ALLOC_NAME];
    u32 allocations_num;
    b8 print_new_allocations;
} ArenaDebugInfo;

void DEBUG_ArenaPrintAllocations(Arena* arena);
void DEBUG_SetPrintNewAllocations(Arena* arena, b8 value);
#else
#define DEBUG_ArenaPrintAllocations(arena) // Stub
#endif


