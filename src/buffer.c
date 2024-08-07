#include "buffer.h"
#include "culibc.h"
#include "error.h"
#include "file.h"

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
    // Add one more byte for string null termination \0
    result.ptr = (void*)PushArray(arena, u8, file_size + 1);
    result.length = file_size;

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

    fileContents[file_size] = '\0';
    return result;
}

const char* CStringFromBuffer(Buffer* buffer)
{
    const char* c_string = (const char*)buffer->ptr;
    return c_string;
}
