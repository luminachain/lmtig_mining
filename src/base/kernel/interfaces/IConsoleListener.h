

#ifndef XMRIG_ICONSOLELISTENER_H
#define XMRIG_ICONSOLELISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class IConsoleListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IConsoleListener)

    IConsoleListener()          = default;
    virtual ~IConsoleListener() = default;

    virtual void onConsoleCommand(char command) = 0;
};


} 


#endif 
