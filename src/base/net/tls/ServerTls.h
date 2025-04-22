


#ifndef XMRIG_SERVERTLS_H
#define XMRIG_SERVERTLS_H


using BIO       = struct bio_st;
using SSL       = struct ssl_st;
using SSL_CTX   = struct ssl_ctx_st;



#include "base/tools/Object.h"


namespace xmrig {


class ServerTls
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(ServerTls)

    ServerTls(SSL_CTX *ctx);
    virtual ~ServerTls();

    static bool isHTTP(const char *data, size_t size);
    static bool isTLS(const char *data, size_t size);

    bool send(const char *data, size_t size);
    void read(const char *data, size_t size);

protected:
    virtual bool write(BIO *bio)                = 0;
    virtual void parse(char *data, size_t size) = 0;
    virtual void shutdown()                     = 0;

private:
    void read();

    BIO *m_read     = nullptr;
    BIO *m_write    = nullptr;
    bool m_ready    = false;
    SSL *m_ssl      = nullptr;
    SSL_CTX *m_ctx;
};


} 


#endif 
