#include "types_str.h"
#include "culibc.h"
#include "assert.h"

char* CStrFromMemSizeA(Arena* arena, u64 size)
{
    const int buffer_size = 10;
    char* str = PushArray(arena, char, buffer_size);
    CStrFromMemSize(str, buffer_size, size);
    return str;
}

void  CStrFromMemSize(char* out, u64 out_size, u64 size)
{
    assert(out_size >= 10);
    if (size >= Terabytes(1)) {
        cu_snprintf(out, out_size, "%.2f TB", size / (double)Terabytes(1));
    } else if (size >= Gigabytes(1)) {
        cu_snprintf(out, out_size, "%.2f GB", size / (double)Gigabytes(1));
    } else if (size >= Megabytes(1)) {
        cu_snprintf(out, out_size, "%.2f MB", size / (double)Megabytes(1));
    } else if (size >= Kilobytes(1)) {
        cu_snprintf(out, out_size, "%.2f KB", size / (double)Kilobytes(1));
    } else {
        cu_snprintf(out, out_size, "%llu Bytes", size);
    }
}