

#ifndef XMRIG_ISIGNALLISTENER_H
#define XMRIG_ISIGNALLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class String;


class ISignalListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(ISignalListener)

    ISignalListener()           = default;
    virtual ~ISignalListener()  = default;

    virtual void onSignal(int signum) = 0;
};


} 


#endif 
