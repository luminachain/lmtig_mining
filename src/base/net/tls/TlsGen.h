

#ifndef XMRIG_TLSGEN_H
#define XMRIG_TLSGEN_H


#include "base/tools/Object.h"
#include "base/tools/String.h"


using EVP_PKEY  = struct evp_pkey_st;
using X509      = struct x509_st;


namespace xmrig {


class TlsGen
{
public:
    XMRIG_DISABLE_COPY_MOVE(TlsGen)

    TlsGen() : m_cert("cert.pem"), m_certKey("cert_key.pem") {}
    ~TlsGen();

    inline const String &cert() const       { return m_cert; }
    inline const String &certKey() const    { return m_certKey; }

    void generate(const char *commonName = nullptr);

private:
    bool generate_x509(const char *commonName);
    bool write();

    const String m_cert;
    const String m_certKey;
    EVP_PKEY *m_pkey    = nullptr;
    X509 *m_x509        = nullptr;
};


} 


#endif 
