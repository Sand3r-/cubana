#include "culibc.h"
#include "SDL_stdinc.h"

i32 cu_snprintf(char *text, size_t maxlen, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    i32 result = cu_vsnprintf(text, maxlen, fmt, args);
    va_end(args);
    return result;
}

i32 cu_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap)
{
    return SDL_vsnprintf(text, maxlen, fmt, ap);
}

i32 cu_asprintf(char **strp, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    i32 result = cu_vasprintf(strp, fmt, args);
    va_end(args);
    return result;
}

i32 cu_vasprintf(char **strp, const char *fmt, va_list ap)
{
    return SDL_vasprintf(strp, fmt, ap);
}

size_t cu_strlen(const char *string)
{
    return SDL_strlen(string);
}

size_t cu_strcmp(const char* str1, const char* str2)
{
    return SDL_strcmp(str1, str2);
}

void cu_memset(void* dst, int c, size_t len)
{
    SDL_memset(dst, c, len);
}
