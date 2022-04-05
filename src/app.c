#include "app.h"
#include "argparser.h"
#include "errorcodes.h"
#include "log/log.h"
#include "file.h"

#define ReturnOnFailure(result) do { \
    int error = result; \
    if (error != CU_SUCCESS) \
    { \
        L_ERROR("Application failed at error: %s", ErrorCodeToString(result)); \
        return error; \
    } \
    } while(0);

static File g_log_file;
static int InitLogger()
{
    log_set_level(0);
    log_set_quiet(0);
    g_log_file = FileOpen("./log.txt", "ab");
    if (!g_log_file.valid)
    {
        L_ERROR(g_log_file.error_msg);
        return CU_LOG_FILE_ERROR;
    }
    log_add_fp(&g_log_file, 0);
    return CU_SUCCESS;
}

static int Init(int argc, char* argv[])
{
    ReturnOnFailure(InitLogger());
    L_INFO("Test");
    CmdArgs cmd_args = ParseCommandLine(argc, argv);
    return CU_SUCCESS;
}

static int GameLoop()
{
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
    return CU_SUCCESS;
}

int Run(int argc, char* argv[])
{
    ReturnOnFailure(Init(argc, argv));
    ReturnOnFailure(GameLoop());
    ReturnOnFailure(Shutdown());
    return CU_SUCCESS;
}