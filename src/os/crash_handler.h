#pragma once

#if defined __cplusplus
    #define EXTERN extern "C"
#else
    #define EXTERN extern
#endif

typedef void(*CrashCallback)(void);

// Initializes an OS-specific crash handling mechanism
EXTERN void InitializeCrashHandlers(void);
// Registers a callback to be invoked when program crashes
EXTERN void RegisterCrashCallback(CrashCallback callback);
