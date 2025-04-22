

#ifndef XMRIG_ICUDARUNNER_H
#define XMRIG_ICUDARUNNER_H


#include "base/tools/Object.h"


#include <cstdint>


namespace xmrig {


class Job;


class ICudaRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE(ICudaRunner)

    ICudaRunner()          = default;
    virtual ~ICudaRunner() = default;

    virtual size_t intensity() const                                                = 0;
    virtual size_t roundSize() const                                                = 0;
    virtual size_t processedHashes() const                                          = 0;
    virtual bool init()                                                             = 0;
    virtual bool run(uint32_t startNonce, uint32_t *rescount, uint32_t *resnonce)   = 0;
    virtual bool set(const Job &job, uint8_t *blob)                                 = 0;
    virtual void jobEarlyNotification(const Job&)                                   = 0;
};


} 


#endif 
