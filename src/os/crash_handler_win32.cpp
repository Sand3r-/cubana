#include "crash_handler.h"
#include <cassert>
#include <csignal>
#include <string>

extern "C" {
    #include "log/log.h"
    #include "memory/thread_scratch.h"
}

struct SignalDescription
{
    const char* name;
    const char* desc;
};

static SignalDescription signalDescriptions[] = {
    {"SIGINT", "Interrupt"},
    {"SIGILL", "Illegal instruction"},
    {"SIGSEGV", "Segment violation"},
    {"SIGTERM", "Software termination signal"},
    {"SIGBREAK", "Ctrl-Break sequence"},
    {"SIGABRT", "Abnormal termination triggered by abort call"},
    {"SIGABRT_COMPAT", "SIGABRT compatible with other platforms, same as SIGABRT"},
    {"UKNOWN", "Unknown signal"}
};

static SignalDescription SignalDescriptionFromSignal(int signal)
{
    switch(signal)
    {
        case SIGINT: return signalDescriptions[0];
        case SIGILL: return signalDescriptions[1];
        case SIGSEGV: return signalDescriptions[2];
        case SIGTERM: return signalDescriptions[3];
        case SIGBREAK: return signalDescriptions[4];
        case SIGABRT: return signalDescriptions[5];
        case SIGABRT_COMPAT: return signalDescriptions[6];
        default: return signalDescriptions[7];
    }
}

#define MAX_CRASH_CALLBACKS_NUM 8

static CrashCallback callbacks[MAX_CRASH_CALLBACKS_NUM];
static int crash_callback_count = 0;

static const char* CharFromWchar(const wchar_t* in) {
    size_t len = wcslen(in);
    size_t converted = 0;
    char* cstr = PushArray(GetScratchArena(), char, len + 1);
    wcstombs_s(&converted, cstr, len + 1, in, len);
    return cstr;
}

static void CallCallbacksAndTerminate()
{
    for (int i = 0; i < crash_callback_count; i++)
    {
        callbacks[i]();
    }
    L_FATAL("Exiting cubana.");
    std::exit(EXIT_FAILURE);
}

static void TerminateHandler()
{
    L_FATAL("std::terminate() called");
    CallCallbacksAndTerminate();
}
static void InvalidParameterHandler(const wchar_t* expression,
    const wchar_t* function, const wchar_t* file,
    unsigned int line, uintptr_t pReserved)
{
    const char* expression_ = CharFromWchar(expression);
    const char* function_ = CharFromWchar(function);
    const char* file_ = CharFromWchar(file);
    L_FATAL("Invalid parameter detected in function %s (file %s, line %d).",
        function_, file_, line);
    L_FATAL("Expression: %s", expression_);
    CallCallbacksAndTerminate();
}

static void SignalHandler(int signal)
{
    auto sig_desc = SignalDescriptionFromSignal(signal);
    L_FATAL("Signal %s (%s) received.", sig_desc.name, sig_desc.desc);
    CallCallbacksAndTerminate();
}

void InitializeCrashHandlers()
{
    std::set_terminate(TerminateHandler);
    _set_invalid_parameter_handler(InvalidParameterHandler);
    std::signal(SIGSEGV, SignalHandler);
    std::signal(SIGABRT, SignalHandler);
    std::signal(SIGILL, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    std::signal(SIGBREAK, SignalHandler);
    std::signal(SIGABRT_COMPAT, SignalHandler);
}

void RegisterCrashCallback(CrashCallback callback)
{
    assert(crash_callback_count < MAX_CRASH_CALLBACKS_NUM && "Too many crash callbacks");
    callbacks[crash_callback_count++] = callback;
}
