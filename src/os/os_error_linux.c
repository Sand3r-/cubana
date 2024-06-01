#include "os_error.h"
#include "log/log.h"
#include <errno.h>

void cu_os_print_last_error()
{
    L_ERROR("Error id: %d", errno);
    L_ERROR("Error message: %s", strerror(errno));
}