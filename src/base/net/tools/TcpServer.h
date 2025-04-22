

#ifndef XMRIG_TCPSERVER_H
#define XMRIG_TCPSERVER_H


#include <uv.h>


#include "base/tools/Object.h"


namespace xmrig {


class ITcpServerListener;
class String;


class TcpServer
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(TcpServer)

    TcpServer(const String &host, uint16_t port, ITcpServerListener *listener);
    ~TcpServer();

    int bind();

private:
    void create(uv_stream_t *stream, int status);

    static void onConnection(uv_stream_t *stream, int status);

    const String &m_host;
    int m_version   = 0;
    ITcpServerListener *m_listener;
    sockaddr_storage m_addr{};
    uint16_t m_port;
    uv_tcp_t *m_tcp;
};


} 


#endif 
