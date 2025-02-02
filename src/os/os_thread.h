#pragma once
#include "types.h"

// move it to OS_Specific header
#ifdef WIN32
typedef HANDLE cu_thread_handle;
#else
typedef int    cu_thread_handle;
#endif

typedef struct cu_thread
{
    cu_thread_id     id;
    cu_thread_handle handle;
    const char*      name;
    cu_thread_fn     function;
    void*            data;
} cu_thread;
typedef u64 cu_thread_id;
typedef int (* cu_thread_fn) (void *data);

void cu_create_thread(const char* name, cu_thread_fn function, void* data);
// void cu_join_thread();
cu_thread_id cu_get_thread_id();
void cu_sleep(u32 ms);
void cu_set_thread_name(const char* name);
void cu_set_thread_priority(u32 priority);
void cu_set_thread_affinity(u32 affinity);
void cu_get_thread_affinity(u32* affinity);
u32 cu_os_get_current_thread_id();
u32 cu_os_get_current_thread_priority();
