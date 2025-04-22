

#include <uv.h>


#include "base/net/https/HttpsServer.h"
#include "base/net/https/HttpsContext.h"
#include "base/net/tls/TlsContext.h"
#include "base/net/tools/NetBuffer.h"


xmrig::HttpsServer::HttpsServer(const std::shared_ptr<IHttpListener> &listener) :
    m_listener(listener)
{
}


xmrig::HttpsServer::~HttpsServer()
{
    HttpContext::closeAll();

    delete m_tls;
}


bool xmrig::HttpsServer::setTls(const TlsConfig &config)
{
    m_tls = TlsContext::create(config);

    return m_tls != nullptr;
}


void xmrig::HttpsServer::onConnection(uv_stream_t *stream, uint16_t)
{
    auto ctx = new HttpsContext(m_tls, m_listener);
    uv_accept(stream, ctx->stream());

    uv_read_start(ctx->stream(), NetBuffer::onAlloc, onRead); 
}


void xmrig::HttpsServer::onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    auto ctx = static_cast<HttpsContext*>(stream->data);
    if (nread >= 0) {
        ctx->append(buf->base, static_cast<size_t>(nread));
    }
    else {
        ctx->close();
    }

    NetBuffer::release(buf);
}
