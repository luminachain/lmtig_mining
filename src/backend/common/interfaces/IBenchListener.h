

#ifndef XMRIG_IBENCHLISTENER_H
#define XMRIG_IBENCHLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class IBackend;


class IBenchListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IBenchListener)

    IBenchListener()            = default;
    virtual ~IBenchListener()   = default;

    virtual void onBenchDone(uint64_t result, uint64_t diff, uint64_t ts)               = 0;
    virtual void onBenchReady(uint64_t ts, uint32_t threads, const IBackend *backend)   = 0;
};


} 


#endif 
