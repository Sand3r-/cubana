#include "app.h"
#include "argparser.h"
#include "errorcodes.h"
#include "log/log.h"

#define ReturnOnFailure(result) do { \
    int error = result; \
    if (error != CU_SUCCESS) \
        L_ERROR("Application failed at error: %s", ErrorCodeToString(result)); \
        return error; \
    } while(0);

static int Init(int argc, char* argv[])
{
    CmdArgs cmd_args = ParseCommandLine(argc, argv);
    return CU_SUCCESS;
}

static int GameLoop()
{
    return CU_SUCCESS;
}

int Run(int argc, char* argv[])
{
    ReturnOnFailure(Init(argc, argv));
    ReturnOnFailure(GameLoop());
    return CU_SUCCESS;
}