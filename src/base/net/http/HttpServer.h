

#ifndef XMRIG_HTTPSERVER_H
#define XMRIG_HTTPSERVER_H


#include "base/kernel/interfaces/ITcpServerListener.h"
#include "base/tools/Object.h"


#include <memory>


namespace xmrig {


class IHttpListener;


class HttpServer : public ITcpServerListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(HttpServer)

    HttpServer(const std::shared_ptr<IHttpListener> &listener);
    ~HttpServer() override;

protected:
    void onConnection(uv_stream_t *stream, uint16_t port) override;

private:
    std::weak_ptr<IHttpListener> m_listener;
};


} 


#endif 

