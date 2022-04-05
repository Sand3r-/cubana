#pragma once
#include <stdarg.h>
#include "types.h"

i32 cu_snprintf(char *text, size_t maxlen, const char *fmt, ...);
i32 cu_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap);
i32 cu_asprintf(char **strp, const char *fmt, ...);
i32 cu_vasprintf(char **strp, const char *fmt, va_list ap);
size_t cu_strlen(const char *string);