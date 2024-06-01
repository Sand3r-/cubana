#pragma once
#include "types.h"

// Reserve (size) bytes of memory. If you don't want to set speicifc base address
// you can set base_ptr to NULL.
void* cu_os_reserve(void* base_ptr, u64 size);
// Commit (size) bytes of memory.
b32   cu_os_commit(void *ptr, u64 size);
// Decommit (size) bytes of memory.
void  cu_os_decommit(void *ptr, u64 size);
// Release (size) bytes of memory. On windows, size is ignored.
void  cu_os_release(void *ptr, u64 size);
// Get system's page size.
u64   cu_os_get_page_size(void);
