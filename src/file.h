#pragma once
#include "types.h"

struct SDL_RWops;

typedef struct File
{
    struct SDL_RWops* _io;
    const char* error_msg;
    b8 valid;
} File;

File FileOpen(const char* file, const char* mode);
u64 FileRead(File* file, void *buffer, u64 obj_size, u64 obj_num);
u64 FileWrite(File* file, void *buffer, u64 obj_size, u64 obj_num);
u64 FileWriteStr(File* file, const char* string);
u64 FileWriteStrf(File* file, const char* fmt, ...);
u64 FileWriteStrfv(File* file, const char* fmt, va_list args);
u64 FileWriteLine(File* file, const char* string);
u64 FileWriteLinef(File* file, const char* fmt, ...);
s64 FileSeek(File* file, s64 offset, int whence);
s64 FileTell(File* file);
i32 FileClose(File* file);
s64 FileSize(File* file);