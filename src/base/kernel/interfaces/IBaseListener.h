

#ifndef XMRIG_IBASELISTENER_H
#define XMRIG_IBASELISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class Config;


class IBaseListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IBaseListener)

    IBaseListener()             = default;
    virtual ~IBaseListener()    = default;

    virtual void onConfigChanged(Config *config, Config *previousConfig) = 0;
};


} 


#endif 
