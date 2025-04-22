


#ifndef XMRIG_HTTPSCONTEXT_H
#define XMRIG_HTTPSCONTEXT_H


using BIO = struct bio_st;
using SSL = struct ssl_st;


#include "base/net/http/HttpContext.h"
#include "base/net/tls/ServerTls.h"


namespace xmrig {


class TlsContext;


class HttpsContext : public HttpContext, public ServerTls
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(HttpsContext)

    HttpsContext(TlsContext *tls, const std::weak_ptr<IHttpListener> &listener);
    ~HttpsContext() override;

    void append(char *data, size_t size);

protected:
    
    bool write(BIO *bio) override;
    void parse(char *data, size_t size) override;
    void shutdown() override;

    
    void write(std::string &&data, bool close) override;

private:
    enum TlsMode : uint32_t {
      TLS_AUTO,
      TLS_OFF,
      TLS_ON
    };

    bool m_close    = false;
    TlsMode m_mode  = TLS_AUTO;
};


} 


#endif 

