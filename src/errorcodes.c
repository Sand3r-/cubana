#include "errorcodes.h"

static const char *error_code_strings[] = {
  "CU_SUCCESS", "CU_FAILURE"
};

const char* ErrorCodeToString(int error_code)
{
    return error_code_strings[error_code];
}