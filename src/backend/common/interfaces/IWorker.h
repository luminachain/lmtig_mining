

#ifndef XMRIG_IWORKER_H
#define XMRIG_IWORKER_H


#include "base/tools/Object.h"


#include <cstdint>
#include <cstddef>


namespace xmrig {


class Job;
class VirtualMemory;


class IWorker
{
public:
    XMRIG_DISABLE_COPY_MOVE(IWorker)

    IWorker()           = default;
    virtual ~IWorker()  = default;

    virtual bool selfTest()                                                                         = 0;
    virtual const VirtualMemory *memory() const                                                     = 0;
    virtual size_t id() const                                                                       = 0;
    virtual size_t intensity() const                                                                = 0;
    virtual size_t threads() const                                                                  = 0;
    virtual void hashrateData(uint64_t &hashCount, uint64_t &timeStamp, uint64_t &rawHashes) const  = 0;
    virtual void jobEarlyNotification(const Job &job)                                               = 0;
    virtual void start()                                                                            = 0;
};


} 


#endif 
