#include "os_memory.h"
#include <memoryapi.h>
#include <sysinfoapi.h>

void* cu_os_reserve(u64 size)
{
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

b32 cu_os_commit(void *ptr, u64 size)
{
    return (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
}

void cu_os_decommit(void *ptr, u64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

void cu_os_release(void *ptr, u64 size)
{
    // Size needs to be fixed 0 when MEM_RELEASE is used
    // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree
    VirtualFree(ptr, 0, MEM_RELEASE);
}

u64 cu_os_get_page_size(void)
{
    SYSTEM_INFO sysinfo = {0};
    GetSystemInfo(&sysinfo);
    return sysinfo.dwPageSize;
}
