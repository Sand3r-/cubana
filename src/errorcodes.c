#include "errorcodes.h"

static const char *error_code_strings[] = {
  "CU_SUCCESS", "CU_FAILURE", "CU_LOG_FILE_ERROR", "CU_FILE_CLOSE_ERROR",
  "CU_INITIALIZATION_ERROR", "CU_WINDOW_FAILURE"
};

const char* ErrorCodeToString(int error_code)
{
    return error_code_strings[error_code];
}