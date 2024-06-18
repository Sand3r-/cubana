#include "buffer.h"
#include "culibc.h"
#include "error.h"
#include "file.h"

Buffer AllocBuffer(Arena* arena, size_t size, const char* alloc_name)
{
    Buffer buffer = {
        .ptr = (void*)PushArray(arena, u8, size),
        .length = size
    };
    return buffer;
}

Buffer BufferFromFile(Arena* arena, const char* filename)
{
    Buffer result = {0};
    File file = FileOpen(filename, "rb");
    if (!file.valid)
    {
        ERROR("%s", file.error_msg);
        return result;
    }

    ArenaMarker marker = ArenaMarkerCreate(arena);
    s64 file_size =  FileSize(&file);
    result = AllocBuffer(arena, file_size, filename);
    char* fileContents = (char*)result.ptr;

    s64 totaj_objs_read = 0, objs_read = 1;
    char* buffer = fileContents;
    while (totaj_objs_read < file_size && objs_read != 0)
    {
        objs_read = FileRead(&file, buffer, 1, (file_size - totaj_objs_read));
        totaj_objs_read += objs_read;
        buffer += objs_read;
    }
    FileClose(&file);
    if (totaj_objs_read != file_size)
    {
        result.ptr = NULL;
        result.length = 0;
        ArenaMarkerRollback(marker);
        return result;
    }

    return result;
}

const char* CStringFromBuffer(Arena* arena, Buffer* buffer)
{
    char* c_string = PushArray(arena, char, buffer->length + 1);
    cu_strlcpy(c_string, buffer->ptr, buffer->length + 1);
    return c_string;
}