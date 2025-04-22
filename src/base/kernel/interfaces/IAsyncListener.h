

#ifndef XMRIG_IASYNCLISTENER_H
#define XMRIG_IASYNCLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class Async;


class IAsyncListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IAsyncListener)

    IAsyncListener()            = default;
    virtual ~IAsyncListener()   = default;

    virtual void onAsync()      = 0;
};


} 


#endif 
