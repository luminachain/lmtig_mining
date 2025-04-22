

#ifndef XMRIG_ITCPSERVERLISTENER_H
#define XMRIG_ITCPSERVERLISTENER_H


#include <stdint.h>


typedef struct uv_stream_s uv_stream_t;


namespace xmrig {


class String;


class ITcpServerListener
{
public:
    virtual ~ITcpServerListener() = default;

    virtual void onConnection(uv_stream_t *stream, uint16_t port) = 0;
};


} 


#endif 
