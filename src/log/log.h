/**
 * Copyright (c) 2020 rxi, 2022 Michal Gallus
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

struct File;

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define L_TRACE(...) log_log(LOG_TRACE, FILE_BASENAME, __LINE__, __VA_ARGS__)
#define L_DEBUG(...) log_log(LOG_DEBUG, FILE_BASENAME, __LINE__, __VA_ARGS__)
#define L_INFO(...)  log_log(LOG_INFO,  FILE_BASENAME, __LINE__, __VA_ARGS__)
#define L_WARN(...)  log_log(LOG_WARN,  FILE_BASENAME, __LINE__, __VA_ARGS__)
#define L_ERROR(...) log_log(LOG_ERROR, FILE_BASENAME, __LINE__, __VA_ARGS__)
#define L_FATAL(...) log_log(LOG_FATAL, FILE_BASENAME, __LINE__, __VA_ARGS__)

const char* log_level_string(int level);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(struct File* file, int level);
int log_init();

void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif
