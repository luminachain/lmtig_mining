

#ifndef XMRIG_ILOGBACKEND_H
#define XMRIG_ILOGBACKEND_H


#include "base/tools/Object.h"


#include <cstdarg>
#include <cstddef>


namespace xmrig {


class ILogBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE(ILogBackend)

    ILogBackend()           = default;
    virtual ~ILogBackend()  = default;

    virtual void print(uint64_t timestamp, int level, const char *line, size_t offset, size_t size, bool colors) = 0;
};


} 


#endif 
