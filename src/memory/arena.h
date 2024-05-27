#pragma once
#include "types.h"

typedef struct Arena {
    void* base;
    u64 pos;
    u64 size;
    u64 capacity;

#ifdef DEBUG_ARENA_ALLOCATIONS
#define TRACABLE_ALLOCS_NUM 4096
#define MAX_ALLOC_LOC_NAME 256
    u8* allocation_ptrs[TRACABLE_ALLOCS_NUM];
    char allocation_names[TRACABLE_ALLOCS_NUM][MAX_ALLOC_LOC_NAME];
    u32 allocations_num;
#endif
} Arena;

Arena ArenaInitialise(u64 size, u64 capacity);
void ArenaShutdown(Arena* arena);

#ifndef DEBUG_ARENA_ALLOCATIONS
void* ArenaPushNoZero(Arena* arena, u64 size);
void* ArenaPush(Arena* arena, u64 size);
#else
void* ArenaPushNoZero(Arena* arena, u64 size, const char* file, int line);
void* ArenaPush(Arena* arena, u64 size, const char* file, int line);
#endif

void ArenaPopTo(Arena* arena, u64 pos);
void ArenaPop(Arena* arena, u64 size);
void ArenaReset(Arena* arena);

// Helpers
#ifndef DEBUG_ARENA_ALLOCATIONS
#define PushArrayNoZero(arena, type, count) (type*)ArenaPushNoZero((arena), sizeof(type) * (count))
#define PushArray(arena, type, count)       (type*)ArenaPush((arena), sizeof(type) * (count))
#else
#define PushArrayNoZero(arena, type, count) (type*)ArenaPushNoZero((arena), sizeof(type) * (count), FILE_BASENAME, __LINE__)
#define PushArray(arena, type, count)       (type*)ArenaPush((arena), sizeof(type) * (count), FILE_BASENAME, __LINE__)
#endif

#define PushStructNoZero(arena, type)       (type*)PushArrayNoZero((arena), sizeof(type), 1)
#define PushStruct(arena, type)             (type*)PushArray((arena), sizeof(type), 1)

#ifdef DEBUG_ARENA_ALLOCATIONS
void DEBUG_ArenaPrintAllocations(Arena* arena);
#endif

