#include "os_thread.h"
#include <windows.h>

typedef struct cu_thread
{
    HANDLE handle;
} cu_thread;

static DWORD WINAPI cu_thread_wrapper(void* data)
{
    cu_thread_info* info = data;
    info->func(info->data);

    return TRUE;
}

cu_thread cu_create_thread(cu_thread_info* info, void *(*func)(void *data), void *data)
{
    info->func = func;
    info->data = data;
    cu_thread thread;
    thread.handle = CreateThread(NULL, info ? info->stack_size : 0, cu_thread_wrapper, info, 0, NULL);
    return thread;
}

void cu_set_affinity(cu_thread* thread, int core)
{
    SetThreadAffinityMask(thread->handle, 1 << core);
}