

#ifndef XMRIG_IMEMORYPOOL_H
#define XMRIG_IMEMORYPOOL_H


#include "base/tools/Object.h"


#include <cstddef>
#include <cstdint>


namespace xmrig {


class IMemoryPool
{
public:
    XMRIG_DISABLE_COPY_MOVE(IMemoryPool)

    IMemoryPool()           = default;
    virtual ~IMemoryPool()  = default;

    virtual bool isHugePages(uint32_t node) const       = 0;
    virtual uint8_t *get(size_t size, uint32_t node)    = 0;
    virtual void release(uint32_t node)                 = 0;
};


} 



#endif 
