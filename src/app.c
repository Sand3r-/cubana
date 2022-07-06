#include "app.h"
#include "argparser.h"
#include "error.h"
#include "log/log.h"
#include "external.h"
#include "file.h"
#include "game.h"
#include "gamepad.h"
#include "input.h"
#include "math/mat.h"
#include "memory/stackallocator.h"
#include "memory/linearallocator.h"
#include "platform.h"
#include "renderer/renderer.h"
#include "timer.h"
#include "window.h"
#include <cimgui.h>
#include <cimgui_impl.h>
#include <cim3d.h>

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
    Game game;
    Window window;
    CmdArgs cmd_args;
} g_app;

static void DEBUG_TestCode(void)
{

}

static File g_log_file;
static int InitLogger(void)
{
    log_set_level(0);
    log_set_quiet(false);
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

static int InitWindow(void)
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

static int InitRenderer(void)
{
    return RendererInit(g_app.window);
}

static int InitGame(void)
{
    return GameInit(&g_app.game);
}

static int Init(int argc, char* argv[])
{
    ReturnOnFailure(InitLogger());
    ReturnOnFailure(InitExternalLibs());
    ReturnOnFailure(InitConfig(argc, argv));
    ReturnOnFailure(StackAllocatorInit(true)); // Enable debug
    ReturnOnFailure(LinearAllocatorInit(true)); // Enable debug
    ReturnOnFailure(InitWindow());
    ReturnOnFailure(InitRenderer());
    ReturnOnFailure(InitGame());

    DEBUG_StopWatchdog();
    DEBUG_TestCode();

    ComputeDeltaTime(); // Run once to initialise

    return CU_SUCCESS;
}

static f32 ComputeDeltaTime(void)
{
    static u64 old_ticks = 0;
    u64 new_ticks = GetPerformanceCounter();
    f32 delta = (new_ticks - old_ticks) * 1000 / (f32)GetPerformanceFrequency();
    old_ticks = new_ticks;

    return delta;
}

void DrawPerformanceStatistics(f32 delta)
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    igBegin("Latency & FPS", NULL, flags);
    igText("Frame time [ms]: %f", delta);
    igText("FPS: %f", 1000.f / (delta));
    igEnd();
}

static int AppLoop(void)
{
    b8 done = false;
    while (!done)
    {
        done = ProcessPlatformEvents();
        f32 delta = ComputeDeltaTime();
        DrawPerformanceStatistics(delta);
        GameUpdate(&g_app.game, delta);
        RendererDraw();
        ResetInput();
        DEBUG_TestCode();
    }
    return CU_SUCCESS;
}

static int Shutdown(void)
{
    GameDestroy(&g_app.game);
    CloseGamepadControllers();
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
    ReturnOnFailure(AppLoop());
    ReturnOnFailure(Shutdown());
    return CU_SUCCESS;
}