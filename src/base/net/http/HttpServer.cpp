


#include <functional>
#include <uv.h>


#include "base/net/http/HttpServer.h"
#include "3rdparty/llhttp/llhttp.h"
#include "base/net/http/HttpContext.h"
#include "base/net/tools/NetBuffer.h"


xmrig::HttpServer::HttpServer(const std::shared_ptr<IHttpListener> &listener) :
    m_listener(listener)
{
}


xmrig::HttpServer::~HttpServer()
{
    HttpContext::closeAll();
}


void xmrig::HttpServer::onConnection(uv_stream_t *stream, uint16_t)
{
    auto ctx = new HttpContext(HTTP_REQUEST, m_listener);
    uv_accept(stream, ctx->stream());

    uv_read_start(ctx->stream(), NetBuffer::onAlloc,
        [](uv_stream_t *tcp, ssize_t nread, const uv_buf_t *buf)
        {
            auto ctx = static_cast<HttpContext*>(tcp->data);

            if (nread < 0 || !ctx->parse(buf->base, static_cast<size_t>(nread))) {
                ctx->close();
            }

            NetBuffer::release(buf);
        });
}
