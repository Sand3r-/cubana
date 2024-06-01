#include "os_error.h"
#include "log/log.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void cu_os_print_last_error()
{
    DWORD errorMessageID = GetLastError();
    if (errorMessageID == 0) {
        return; //No error message has been recorded
    }

    LPSTR messageBuffer = NULL;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    L_ERROR("GetLastError() returned: %s", messageBuffer);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
}