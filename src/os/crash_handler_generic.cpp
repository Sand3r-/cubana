#include "crash_handler.h"
extern "C" {
    #include "log/log.h"
}
void InitializeCrashHandlers()
{
    L_WARN("Crash handling mechanism wasn't implemented on this system");
}

void RegisterCrashCallback(CrashCallback callback)
{
    L_WARN("The crash callback couldn't have been registered");
}
