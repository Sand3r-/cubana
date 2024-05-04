#pragma once

#if defined __cplusplus
    #define EXTERN extern "C"
#else
    #define EXTERN extern
#endif

EXTERN void InitializeScriptEditor(void);
EXTERN void UpdateScriptEditor(void);