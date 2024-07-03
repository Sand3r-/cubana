#pragma once

#if defined __cplusplus
    #define EXTERN extern "C"
#else
    #define EXTERN extern
#endif

typedef struct Arena Arena;

EXTERN void InitializeScriptEditor(Arena* arena);
EXTERN void UpdateScriptEditor(void);
EXTERN void ReportLuaError(const char* error);
EXTERN void LogToScriptEditorConsole(const char* string);