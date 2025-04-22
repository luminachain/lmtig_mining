

#ifndef XMRIG_FILELOG_H
#define XMRIG_FILELOG_H


#include "base/io/log/FileLogWriter.h"
#include "base/kernel/interfaces/ILogBackend.h"


namespace xmrig {


class FileLog : public ILogBackend
{
public:
    FileLog(const char *fileName);

protected:
    void print(uint64_t timestamp, int level, const char *line, size_t offset, size_t size, bool colors) override;

private:
    FileLogWriter m_writer;
};


} 


#endif 
