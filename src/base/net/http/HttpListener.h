

#ifndef XMRIG_HTTPLISTENER_H
#define XMRIG_HTTPLISTENER_H


#include "base/kernel/interfaces/IHttpListener.h"


namespace xmrig {


class HttpListener : public IHttpListener
{
public:
    inline HttpListener(IHttpListener *listener, const char *tag = nullptr) :
#       ifdef APP_DEBUG
        m_tag(tag),
#       endif
        m_listener(listener)
    {}

protected:
    void onHttpData(const HttpData &data) override;

private:
#   ifdef APP_DEBUG
    const char *m_tag;
#   endif
    IHttpListener *m_listener;
};


} 


#endif 
