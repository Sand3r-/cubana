#pragma once
#include <stdarg.h>
#include <stddef.h>
#include "types.h"

i32 cu_snprintf(char *text, size_t maxlen, const char *fmt, ...);
i32 cu_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap);
i32 cu_asprintf(char **strp, const char *fmt, ...);
i32 cu_vasprintf(char **strp, const char *fmt, va_list ap);
char* cu_strstr(const char *string, const char* substring);
size_t cu_strlen(const char *string);
size_t cu_strcmp(const char* str1, const char* str2);
size_t cu_strncmp(const char* str1, const char* str2, size_t maxlen);
size_t cu_strlcpy(char* dst, const char* src, size_t max_len);
size_t cu_strlcat(char* dst, const char* src, size_t max_len);
int cu_sscanf(const char* str, const char* fmt, ...);
void cu_memset(void* dst, int c, size_t len);
void cu_memcpy(void* dst, void* src, size_t len);
char* cu_getcwd(void);
void cu_freecwdptr(char* path);
