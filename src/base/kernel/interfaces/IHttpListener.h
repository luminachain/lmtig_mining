

#ifndef XMRIG_IHTTPLISTENER_H
#define XMRIG_IHTTPLISTENER_H


#include "base/tools/Object.h"


namespace xmrig {


class HttpData;
class HttpResponse;


class IHttpListener
{
public:
    XMRIG_DISABLE_COPY_MOVE(IHttpListener)

    IHttpListener()             = default;
    virtual ~IHttpListener()    = default;

    virtual void onHttpData(const HttpData &data) = 0;
};


} 


#endif 
