


#include <syslog.h>


#include "base/io/log/backends/SysLog.h"
#include "version.h"


xmrig::SysLog::SysLog()
{
    openlog("lmtig_mining", LOG_PID, LOG_USER);
}


xmrig::SysLog::~SysLog()
{
    closelog();
}


void xmrig::SysLog::print(uint64_t, int level, const char *line, size_t offset, size_t, bool colors)
{
    if (colors) {
        return;
    }

    syslog(level == -1 ? LOG_INFO : level, "%s", line + offset);
}
