


#include <uv.h>


#include "base/kernel/Process.h"


int xmrig::Process::pid()
{
#   if UV_VERSION_HEX >= 0x011200
    return uv_os_getpid();
#   else
    return GetCurrentProcessId();
#   endif
}
