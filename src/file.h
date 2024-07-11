#pragma once
#include "types.h"
#include <stdarg.h>

struct SDL_RWops;
typedef struct Arena Arena;

typedef struct File
{
    struct SDL_RWops* _io;
    const char* error_msg;
    b8 valid;
} File;

// Opens file handle with one of the following modes:
// "r" - open for reading
// "w" - open for writing
// "a" - open for appending
// "rb" - open for reading in binary mode
// "wb" - open for writing in binary mode
// "ab" - open for appending in binary mode
// Returns a file handle structure indicating if the operation was successful
// using a valid field set to 1 and non-null error_msg when valid is set to 0.
File FileOpen(const char* file, const char* mode);

// Returns number of bytes read. Returns 0 if end of stream is encountered
u64 FileRead(File* file, void *buffer, u64 obj_size, u64 obj_num);

// FileWrite* functionsReturns number of bytes written.
u64 FileWrite(File* file, void *buffer, u64 obj_size, u64 obj_num);
u64 FileWriteStr(File* file, const char* string);
u64 FileWriteStrf(File* file, const char* fmt, ...);
u64 FileWriteStrfv(File* file, const char* fmt, va_list args);
u64 FileWriteLine(File* file, const char* string);
u64 FileWriteLinef(File* file, const char* fmt, ...);

// This function seeks to byte offset, relative to whence. whence may be any of
// the following values:
// - RW_SEEK_SET: seek from the beginning of data
// - RW_SEEK_CUR: seek relative to current read point
// - RW_SEEK_END: seek relative to the end of data
// If this stream can not seek, it will return -1.
s64 FileSeek(File* file, s64 offset, int whence);

// Determine the current read/write offset in a file
s64 FileTell(File* file);

// Closes file handle
i32 FileClose(File* file);

// Returns file size or -1 on failure
s64 FileSize(File* file);

// Allocate a buffer and load file contents into a CString
char* CStringFromFile(Arena* arena, const char* file);
