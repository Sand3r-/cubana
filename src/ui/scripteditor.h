#pragma once

#if defined __cplusplus
    #define EXTERN extern "C"
#else
    #define EXTERN extern
#endif

typedef struct Arena Arena;

EXTERN void InitializeScriptEditor(Arena* arena);
EXTERN void UpdateScriptEditor(void);