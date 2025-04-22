

#ifndef XMRIG_SYSLOG_H
#define XMRIG_SYSLOG_H


#include "base/kernel/interfaces/ILogBackend.h"


namespace xmrig {


class SysLog : public ILogBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE(SysLog)

    SysLog();
    ~SysLog() override;

protected:
    void print(uint64_t timestamp, int level, const char *line, size_t offset, size_t size, bool colors) override;
};


} 


#endif 
