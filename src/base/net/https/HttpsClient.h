


#ifndef XMRIG_HTTPSCLIENT_H
#define XMRIG_HTTPSCLIENT_H


using BIO       = struct bio_st;
using SSL_CTX   = struct ssl_ctx_st;
using SSL       = struct ssl_st;
using X509      = struct x509_st;


#include "base/net/http/HttpClient.h"
#include "base/tools/String.h"


namespace xmrig {


class HttpsClient : public HttpClient
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(HttpsClient)

    HttpsClient(const char *tag, FetchRequest &&req, const std::weak_ptr<IHttpListener> &listener);
    ~HttpsClient() override;

    const char *tlsFingerprint() const override;
    const char *tlsVersion() const override;

protected:
    void handshake() override;
    void read(const char *data, size_t size) override;

private:
    void write(std::string &&data, bool close) override;

    bool verify(X509 *cert);
    bool verifyFingerprint(X509 *cert);
    void flush(bool close);

    BIO *m_read                         = nullptr;
    BIO *m_write                        = nullptr;
    bool m_ready                        = false;
    char m_fingerprint[32 * 2 + 8]{};
    SSL *m_ssl                          = nullptr;
    SSL_CTX *m_ctx                      = nullptr;
};


} 


#endif 
