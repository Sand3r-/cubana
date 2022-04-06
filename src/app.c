#include "app.h"
#include "argparser.h"
#include "errorcodes.h"
#include "log/log.h"
#include "file.h"
#include "external.h"
#include "memory/stackallocator.h"
#include "platform.h"
#include "window.h"

#define ReturnOnFailure(result) do { \
    int error = result; \
    if (error != CU_SUCCESS) \
    { \
        L_ERROR("Application failed at error: %s", ErrorCodeToString(result)); \
        return error; \
    } \
    } while(0);

struct Application
{
    Window window;
    CmdArgs cmd_args;
} g_app;

static File g_log_file;
static int InitLogger()
{
    log_set_level(0);
    log_set_quiet(0);
    log_init();
    g_log_file = FileOpen("./log.txt", "ab");
    if (!g_log_file.valid)
    {
        L_ERROR(g_log_file.error_msg);
        return CU_LOG_FILE_ERROR;
    }
    log_add_fp(&g_log_file, 0);
    L_INFO("Logging system initialized");
    return CU_SUCCESS;
}

static int InitConfig(int argc, char* argv[])
{
    g_app.cmd_args = ParseCommandLine(argc, argv);
    L_INFO("Command line parsing finished");
    return CU_SUCCESS;
}

static int InitWindow()
{
    WindowResult window_result = CreateWindow(1024, 768, "Cubana");
    g_app.window = window_result.window;
    if (!window_result.valid)
    {
        L_FATAL("Window couldn't have been created: %s", window_result.error_message);
        return CU_WINDOW_FAILURE;
    }

    iv2 window_size = GetWindowSize(g_app.window);
    L_INFO("Window of size %d x %d has been created", window_size.width, window_size.height);
    return CU_SUCCESS;
}

static int Init(int argc, char* argv[])
{
    ReturnOnFailure(InitLogger());
    ReturnOnFailure(InitExternalLibs());
    ReturnOnFailure(InitConfig(argc, argv));
    ReturnOnFailure(StackAllocatorInit(1)); // Enable debug
    ReturnOnFailure(InitWindow());

    return CU_SUCCESS;
}

static int GameLoop()
{
    b8 done = 0;
    while (!done)
    {
        done = ProcessPlatformEvents();
    }
    return CU_SUCCESS;
}

static int Shutdown()
{
    i32 error_code = FileClose(&g_log_file);
    if (error_code != CU_SUCCESS)
    {
        L_ERROR(g_log_file.error_msg);
        return error_code;
    }
    error_code = StackAllocatorShutdown();
    DestroyWindow(g_app.window);

    return error_code;
}

int Run(int argc, char* argv[])
{
    ReturnOnFailure(Init(argc, argv));
    ReturnOnFailure(GameLoop());
    ReturnOnFailure(Shutdown());
    return CU_SUCCESS;
}