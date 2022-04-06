#pragma once

enum
{
    CU_SUCCESS = 0,
    CU_FAILURE = 1,
    CU_LOG_FILE_ERROR = 2,
    CU_FILE_CLOSE_ERROR = 3,
    CU_INITIALIZATION_ERROR = 4,
    CU_WINDOW_FAILURE = 5,
};

const char* ErrorCodeToString(int error_code);