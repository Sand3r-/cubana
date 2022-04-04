#pragma once
#include "types.h"

typedef struct CmdArgs
{
    b8 fullscreen;
} CmdArgs;

CmdArgs ParseCommandLine(int argc, char* argv[]);
