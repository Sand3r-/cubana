#include "buffer.h"
#include "error.h"
#include "file.h"
#include "memory/stackallocator.h"

Buffer AllocBuffer(size_t size, const char* alloc_name)
{
    Buffer buffer = {
        .ptr = StackMalloc(size, alloc_name),
        .length = size
    };
    return buffer;
}

Buffer FreeBuffer(Buffer buffer)
{
    StackFree(buffer.ptr);
    buffer.ptr = NULL;
    buffer.length = 0;
    return buffer;
}

Buffer File2Buffer(const char* filename)
{
    Buffer result = {0};
    File file = FileOpen(filename, "rb");
    if (!file.valid)
    {
        ERROR("%s", file.error_msg);
        return result;
    }

    s64 fileSize =  FileSize(&file);
    result = AllocBuffer(fileSize, filename);
    char* fileContents = (char*)result.ptr;

    s64 totalObjectsRead = 0, objectsRead = 1;
    char* buffer = fileContents;
    while (totalObjectsRead < fileSize && objectsRead != 0)
    {
        objectsRead = FileRead(&file, buffer, 1, (fileSize - totalObjectsRead));
        totalObjectsRead += objectsRead;
        buffer += objectsRead;
    }
    FileClose(&file);
    if (totalObjectsRead != fileSize)
    {
        result = FreeBuffer(result);
        return result;
    }

    return result;
}
