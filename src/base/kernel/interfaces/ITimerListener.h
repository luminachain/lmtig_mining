

#ifndef XMRIG_ITIMERLISTENER_H
#define XMRIG_ITIMERLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class Timer;


class ITimerListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(ITimerListener)

    ITimerListener()            = default;
    virtual ~ITimerListener()   = default;

    virtual void onTimer(const Timer *timer) = 0;
};


} 


#endif 
