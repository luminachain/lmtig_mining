

#ifndef XMRIG_CONSOLELOG_H
#define XMRIG_CONSOLELOG_H


using uv_stream_t = struct uv_stream_s;
using uv_tty_t    = struct uv_tty_s;


#include "base/kernel/interfaces/ILogBackend.h"
#include "base/tools/Object.h"


namespace xmrig {


class Title;


class ConsoleLog : public ILogBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE(ConsoleLog)

    ConsoleLog(const Title &title);
    ~ConsoleLog() override;

protected:
    void print(uint64_t timestamp, int level, const char *line, size_t offset, size_t size, bool colors) override;

private:
    static bool isSupported();

    uv_tty_t *m_tty = nullptr;

#   ifdef XMRIG_OS_WIN
    bool isWritable() const;

    uv_stream_t *m_stream = nullptr;
#   endif
};


} 


#endif 
