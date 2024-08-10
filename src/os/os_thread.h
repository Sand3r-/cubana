#pragma once
#include "types.h"

typedef struct cu_thread cu_thread;

typedef struct cu_thread_info
{
    u64 stack_size;
    void *(*func)(void *data);
    void* data;
} cu_thread_info;

cu_thread cu_create_thread(cu_thread_info* info, void *(*func)(void *data), void *data);
void cu_set_affinity(cu_thread* thread, int core);