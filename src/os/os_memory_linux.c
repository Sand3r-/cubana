#include "os_memory.h"
#include <sys/mman.h>
#include <unistd.h>

void* cu_os_reserve(void* base_ptr, u64 size)
{
    return mmap(base_ptr, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

b32 cu_os_commit(void *ptr, u64 size)
{
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return true;
}

void cu_os_decommit(void *ptr, u64 size)
{
    madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
}

void cu_os_release(void *ptr, u64 size)
{
    munmap(ptr, size);
}

u64 cu_os_get_page_size(void)
{
    return (u64)getpagesize();
}
