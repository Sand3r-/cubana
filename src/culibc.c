#include "culibc.h"
#include "SDL_stdinc.h"
#include "SDL_filesystem.h"

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

char* cu_strstr(const char *string, const char* substring)
{
    return SDL_strstr(string, substring);
}

size_t cu_strlen(const char *string)
{
    return SDL_strlen(string);
}

size_t cu_strcmp(const char* str1, const char* str2)
{
    return SDL_strcmp(str1, str2);
}

size_t cu_strncmp(const char* str1, const char* str2, size_t maxlen)
{
    return SDL_strncmp(str1, str2, maxlen);
}

size_t cu_strlcpy(char* dst, const char* src, size_t max_len)
{
    return SDL_strlcpy(dst, src, max_len);
}

size_t cu_strlcat(char* dst, const char* src, size_t max_len)
{
    return SDL_strlcat(dst, src, max_len);
}

int cu_sscanf(const char* str, const char* fmt, ...)
{
    int rc;
    va_list ap;
    va_start(ap, fmt);
    rc = SDL_vsscanf(str, fmt, ap);
    va_end(ap);
    return rc;
}

void cu_memset(void* dst, int c, size_t len)
{
    SDL_memset(dst, c, len);
}

void cu_memcpy(void* dst, void* src, size_t len)
{
    SDL_memcpy(dst, src, len);
}

char* cu_getcwd(void)
{
    return SDL_GetBasePath();
}
void cu_freecwdptr(char* path)
{
    SDL_free(path);
}

