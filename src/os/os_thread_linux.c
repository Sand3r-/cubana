#include "os_thread.h"
#include <pthreads.h>
#include <sched.h>

typedef struct cu_thread
{
    pthread_t thread;
} cu_thread;

cu_thread cu_create_thread(cu_thread_info* info, void *(*func)(void *data), void *data)
{
    pthread_attr_t attr;
    if (info->stack_size) pthread_attr_setstacksize(&attr, info->stack_size);
    cu_thread thread;
    thread.handle = pthread_create(&thread.thread, &attr, func, data);
    return thread;
}

void cu_set_affinity(cu_thread* thread, int core)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    return pthread_setaffinity_np(thread->thread, sizeof(cpu_set_t), &cpuset);
}