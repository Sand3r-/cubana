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
    static bool gizmos_enabled = true;
    igBegin("Gizmo Enablement", NULL, 0);
    igCheckbox("Enable Gizmos", &gizmos_enabled);
    igEnd();
    static bool inited = false;
    static m4 matrices[5];
    static m4 default_matrix = {
        .rows = {
            { 1.f, 0.f, 0.f, 0.f },
            { 0.f, 1.f           },
            { 0.f, 0.f, 1.f      },
            { 0.f, 0.f, 2.f, 1.f },
        }
    };

    if (inited == false)
    {
        matrices[0] = default_matrix;
        matrices[1] = default_matrix;
        matrices[2] = default_matrix;
        matrices[3] = default_matrix;
        matrices[4] = default_matrix;
        inited = true;
    }
    if (gizmos_enabled)
        im3dGizmo("m0", (float*)matrices[0].elems);
    im3dPushMatrix(matrices[0]);
    im3dPushDrawState();
    im3dSetColor(im3dColor_Gold & 0xFFFFFF99);
    im3dDrawSphereFilled(v3(0.0f), 1.0f, -1);
    im3dPopDrawState();
    im3dPopMatrix();

    if (gizmos_enabled)
        im3dGizmo("m1", (float*)matrices[1].elems);
    im3dPushMatrix(matrices[1]);
    im3dPushDrawState();
    im3dSetColor(im3dColor_Green & 0xFFFFFF99);
    im3dDrawQuad(v3(0.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f));
    im3dPopDrawState();
    im3dPopMatrix();

    if (gizmos_enabled)
        im3dGizmo("m2", (float*)matrices[2].elems);
    im3dPushMatrix(matrices[2]);
    im3dPushDrawState();
    im3dSetColor(im3dColor_Red & 0xFFFFFF99);
    im3dDrawQuadFilled(v3(0.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f));
    im3dPopDrawState();
    im3dPopMatrix();

    if (gizmos_enabled)
        im3dGizmo("m3", (float*)matrices[3].elems);
    im3dPushMatrix(matrices[3]);
    im3dPushDrawState();
    im3dSetColor(im3dColor_Cyan & 0xFFFFFF99);
    im3dDrawAlignedBox(v3(-0.5f), v3(0.5f));
    im3dPopDrawState();
    im3dPopMatrix();

    if (gizmos_enabled)
        im3dGizmo("m4", (float*)matrices[4].elems);
    im3dPushMatrix(matrices[4]);
    im3dPushDrawState();
    im3dSetColor(im3dColor_Orange & 0xFFFFFF99);
    im3dDrawCylinder(v3(0.0f), v3(0.0f, 3.0f, 0.0f), 0.5f, -1);
    im3dPopDrawState();
    im3dPopMatrix();
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

    return CU_SUCCESS;
}

static int AppLoop(void)
{
    b8 done = false;
    while (!done)
    {
        done = ProcessPlatformEvents();
        DEBUG_TestCode();
        GameUpdate(&g_app.game);
        RendererDraw();
        ResetInput();
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