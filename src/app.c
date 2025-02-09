#include "app.h"
#include "argparser.h"
#include "error.h"
#include "event.h"
#include "log/log.h"
#include "external.h"
#include "file.h"
#include "game.h"
#include "gamepad.h"
#include "input.h"
#include "math/mat.h"
#include "memory/arena.h"
#include "memory/thread_scratch.h"
#include "os/crash_handler.h"
#include "platform.h"
#include "replay.h"
#include "renderer/renderer.h"
#include "save.h"
#include "script/scripting.h"
#include "timer.h"
#include "window.h"
#include <cimgui.h>
#include <cim3d.h>

#include <lua.h>
#include <lauxlib.h>

#define ReturnOnFailure(result) do { \
    int error = result; \
    if (error != CU_SUCCESS) \
    { \
        L_ERROR("Application failed at error: %s", ErrorCodeToString(result)); \
        return error; \
    } \
    } while(0);

#define FRAME_ARENAS_NUM 2

struct Application
{
    ReplayBuffer replay_buffers[REPLAY_BUFFER_NUM];
    Game* game;
    Window window;
    CmdArgs cmd_args;
    Arena vk_arena;
    Arena perm_storage_arena;
    Arena frame_arenas[FRAME_ARENAS_NUM];
    u8 frame_arena_index;
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

static int InitArenas(void)
{
    // Init main permament storage arena
    void* base_address = (void*)(Gigabytes(64));
    g_app.perm_storage_arena = ArenaInitialise(base_address, Megabytes(1), Megabytes(1024));

    // Init frame arenas
    base_address = (void*)(Gigabytes(3 * 64));
    g_app.frame_arenas[0] = ArenaInitialise(base_address, Megabytes(16), Megabytes(1024));
    DEBUG_SetPrintNewAllocations(&g_app.frame_arenas[0], false);

    base_address = (void*)(Gigabytes(4 * 64));
    g_app.frame_arenas[1] = ArenaInitialise(base_address, Megabytes(16), Megabytes(1024));
    DEBUG_SetPrintNewAllocations(&g_app.frame_arenas[1], false);

    // Init vk arena
    base_address = (void*)(Gigabytes(5 * 64));
    g_app.vk_arena = ArenaInitialise(base_address, Kilobytes(512), Megabytes(1));

    // Init scratch arena
    ThreadScratchArenaInitialise();

    return CU_SUCCESS;
}

static int InitReplaySystemInfo(void)
{
    ReplaySystemInitInfo info = {
        .replay_buffers = g_app.replay_buffers,
        .arenas = {
            &g_app.perm_storage_arena,
            &g_app.frame_arenas[0],
            &g_app.frame_arenas[1]
        },
    };

    InitReplaySystem(&info);
    L_INFO("Replay system initialized");

    return CU_SUCCESS;
}

static int InitSaveSystemInfo(void)
{
    SaveSystemInitInfo info = {
        .arenas = {
            &g_app.perm_storage_arena,
            &g_app.frame_arenas[0],
            &g_app.frame_arenas[1]
        },
    };

    InitSaveSystem(&info);
    L_INFO("Save system initialized");

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
    return RendererInit(&g_app.vk_arena, g_app.window);
}

static int InitGame(void)
{
    g_app.game = PushStruct(&g_app.perm_storage_arena, Game);
    return GameInit(&g_app.perm_storage_arena, g_app.game);
}

static f32 GetTimeDelta(void)
{
    static u64 old_ticks = 0;
    u64 new_ticks = GetPerformanceCounter();
    f32 delta = (new_ticks - old_ticks) * 1000 / (f32)GetPerformanceFrequency();
    old_ticks = new_ticks;

    return delta;
}

static int Init(int argc, char* argv[])
{
    ReturnOnFailure(InitLogger());
    ReturnOnFailure(InitExternalLibs());
    ReturnOnFailure(InitConfig(argc, argv));
    ReturnOnFailure(InitArenas());
    ReturnOnFailure(InitReplaySystemInfo());
    ReturnOnFailure(InitSaveSystemInfo())
    ReturnOnFailure(InitWindow());
    ReturnOnFailure(InitRenderer());
    ReturnOnFailure(ScriptEngineInit());
    ReturnOnFailure(InitGame());
    InitializeCrashHandlers();

    DEBUG_TestCode();

    GetTimeDelta(); // Run once to initialise

    return CU_SUCCESS;
}

static Arena* GetCurrentFrameArena()
{
    return &g_app.frame_arenas[g_app.frame_arena_index];
}

static void DrawPerformanceStatistics(f32 delta)
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    igBegin("Latency & FPS", NULL, flags);
    igText("Frame time [ms]: %f", delta);
    igText("FPS: %f", 1000.f / (delta));
    igEnd();
}

static void PropagateEvents(void)
{
    Event event;
    while (PollEvent(&event))
    {
        GameProcessEvent(g_app.game, event);
        ScriptEngineProcessEvent(event);
    }
}

static void SwapFrameArenas(void)
{
    g_app.frame_arena_index = !g_app.frame_arena_index;
}

static void ClearCurrentArena(void)
{
    ArenaReset(&g_app.frame_arenas[g_app.frame_arena_index]);
}

static int AppLoop(void)
{
    b8 done = false;
    while (!done)
    {
        ClearCurrentArena();
        done = ProcessPlatformEvents();
        f32 delta = GetTimeDelta();
        Arena* frame_arena = GetCurrentFrameArena();
        UpdateReplaySystem();
        UpdateSaveSystem();
        DrawPerformanceStatistics(delta);
        EmitEvent(CreateEventTick(delta));
        PropagateEvents();
        RendererBeginFrame(frame_arena);
        GameUpdate(frame_arena, g_app.game, delta);
        RendererRender(frame_arena, delta);
        ResetInput();
        SwapFrameArenas();
    }
    return CU_SUCCESS;
}

static int Shutdown(void)
{
    GameDestroy(g_app.game);
    CloseGamepadControllers();
    i32 error_code = FileClose(&g_log_file);
    if (error_code != CU_SUCCESS)
    {
        L_ERROR(g_log_file.error_msg);
        return error_code;
    }
    ArenaShutdown(&g_app.perm_storage_arena);
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