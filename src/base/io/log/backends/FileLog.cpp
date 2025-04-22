


#include "base/io/log/backends/FileLog.h"


#include <cassert>
#include <cstring>


xmrig::FileLog::FileLog(const char *fileName) :
    m_writer(fileName)
{
}


void xmrig::FileLog::print(uint64_t, int, const char *line, size_t, size_t size, bool colors)
{
    if (!m_writer.isOpen() || colors) {
        return;
    }

    assert(strlen(line) == size);

    m_writer.write(line, size);
}
