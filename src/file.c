#include "file.h"
#include "SDL.h"
#include "errorcodes.h"
#include <assert.h>

#define FORMATTED_BUFFER_LENGTH 1024

static u64 _FileWriteStrFormatted(File* file, char* buffer, u32 buffer_length, const char* fmt, va_list args)
{
    i32 str_len = SDL_vsnprintf(buffer, buffer_length, fmt, args);
    assert(str_len < (i32)buffer_length); // Ensure we didn't exceed the buffer
    return SDL_RWwrite(file->_io, buffer, sizeof(char), str_len);
}

File FileOpen(const char* path, const char* mode)
{
    SDL_RWops* sdl_handle = SDL_RWFromFile(path, mode);
    if (sdl_handle == NULL)
    {
        File file = {
            ._io = NULL,
            .valid = 0,
            .error_msg = SDL_GetError()
        };
        return file;
    }

    File file = {
        ._io = sdl_handle,
        .valid = 1,
        .error_msg = NULL
    };

    return file;
}

u64 FileRead(File* file, void *buffer, u64 obj_size, u64 obj_num)
{
    return SDL_RWread(file->_io, buffer, obj_size, obj_num);
}

u64 FileWrite(File* file, void *buffer, u64 obj_size, u64 obj_num)
{
    return SDL_RWwrite(file->_io, buffer, obj_size, obj_num);
}

u64 FileWriteStr(File* file, const char* string)
{
    return SDL_RWwrite(file->_io, string, sizeof(char), SDL_strlen(string));
}

u64 FileWriteStrf(File* file, const char* fmt, ...)
{
    char buffer[FORMATTED_BUFFER_LENGTH];
    va_list var_args;
    va_start(var_args, fmt); // Fill argument list
    int res = _FileWriteStrFormatted(file, buffer, FORMATTED_BUFFER_LENGTH, fmt, var_args);
    va_end(var_args); // Free memory allocated for var_args
    return res;
}

u64 FileWriteStrfv(File* file, const char* fmt, va_list args)
{
    char buffer[FORMATTED_BUFFER_LENGTH];
    return _FileWriteStrFormatted(file, buffer, FORMATTED_BUFFER_LENGTH, fmt, args);
}

u64 FileWriteLine(File* file, const char* string)
{
    return FileWriteStrf(file, "%s\n", string);
}

u64 FileWriteLinef(File* file, const char* fmt, ...)
{
    char buffer[FORMATTED_BUFFER_LENGTH];
    va_list var_args;
    va_start(var_args, fmt); // Fill argument list
    int res = _FileWriteStrFormatted(file, buffer, FORMATTED_BUFFER_LENGTH, fmt, var_args);
    va_end(var_args); // Free memory allocated for var_args
    res += FileWriteStr(file, "\n");
    return res;
}

s64 FileSeek(File* file, s64 offset, int whence)
{
    return SDL_RWseek(file->_io, offset, whence);
}

s64 FileTell(File* file)
{
    return SDL_RWtell(file->_io);
}

i32 FileClose(File* file)
{
    file->error_msg = "File was already closed.";
    file->valid = 0;

    int error_code = SDL_RWclose(file->_io);
    if (error_code)
    {
        file->error_msg = SDL_GetError();
        return CU_FILE_CLOSE_ERROR;
    }

    return CU_SUCCESS;
}
