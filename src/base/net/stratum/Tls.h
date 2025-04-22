

#ifndef XMRIG_CLIENT_TLS_H
#define XMRIG_CLIENT_TLS_H


using BIO       = struct bio_st;
using SSL       = struct ssl_st;
using SSL_CTX   = struct ssl_ctx_st;
using X509      = struct x509_st;


#include "base/net/stratum/Client.h"
#include "base/tools/Object.h"


namespace xmrig {


class Client::Tls
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Tls)

    Tls(Client *client);
    ~Tls();

    bool handshake(const char* servername);
    bool send(const char *data, size_t size);
    const char *fingerprint() const;
    const char *version() const;
    void read(const char *data, size_t size);

private:
    bool send();
    bool verify(X509 *cert);
    bool verifyFingerprint(X509 *cert);

    BIO *m_read     = nullptr;
    BIO *m_write    = nullptr;
    bool m_ready    = false;
    char m_fingerprint[32 * 2 + 8]{};
    Client *m_client;
    SSL *m_ssl      = nullptr;
    SSL_CTX *m_ctx;
};


} 


#endif 
