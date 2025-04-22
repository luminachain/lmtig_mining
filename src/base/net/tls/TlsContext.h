

#ifndef XMRIG_TLSCONTEXT_H
#define XMRIG_TLSCONTEXT_H


#include "base/tools/Object.h"


using SSL_CTX = struct ssl_ctx_st;


namespace xmrig {


class TlsConfig;


class TlsContext
{
public:
    XMRIG_DISABLE_COPY_MOVE(TlsContext)

    ~TlsContext();

    static TlsContext *create(const TlsConfig &config);

    inline SSL_CTX *ctx() const { return m_ctx; }

private:
    TlsContext() = default;

    bool load(const TlsConfig &config);
    bool setCiphers(const char *ciphers);
    bool setCipherSuites(const char *ciphersuites);
    bool setDH(const char *dhparam);
    void setProtocols(uint32_t protocols);

    SSL_CTX *m_ctx = nullptr;
};


} 


#endif 
