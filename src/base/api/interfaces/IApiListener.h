

#ifndef XMRIG_IAPILISTENER_H
#define XMRIG_IAPILISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class IApiRequest;


class IApiListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IApiListener)

    IApiListener()          = default;
    virtual ~IApiListener() = default;

#   ifdef XMRIG_FEATURE_API
    virtual void onRequest(IApiRequest &request) = 0;
#   endif
};


} 


#endif 
