#pragma once
#include "types.h"

void* cu_os_reserve(u64 size);
b32   cu_os_commit(void *ptr, u64 size);
void  cu_os_decommit(void *ptr, u64 size);
void  cu_os_release(void *ptr, u64 size);
u64   cu_os_get_page_size(void);
