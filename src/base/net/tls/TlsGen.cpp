

#include "base/net/tls/TlsGen.h"


#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdexcept>
#include <fstream>


namespace xmrig {


static const char *kLocalhost = "localhost";


static EVP_PKEY *generate_pkey()
{
#   if OPENSSL_VERSION_NUMBER < 0x30000000L || defined(LIBRESSL_VERSION_NUMBER)
    auto pkey = EVP_PKEY_new();
    if (!pkey) {
        return nullptr;
    }

    auto exponent = BN_new();
    auto rsa      = RSA_new();

    
    if (!exponent || !rsa || !BN_set_word(exponent, RSA_F4) || !RSA_generate_key_ex(rsa, 2048, exponent, nullptr) || !EVP_PKEY_assign_RSA(pkey, rsa)) {
        EVP_PKEY_free(pkey);
        BN_free(exponent);
        RSA_free(rsa);

        return nullptr;
    }

    BN_free(exponent);

    return pkey;
#   else
    return EVP_RSA_gen(2048);
#   endif
}


bool isFileExist(const char *fileName)
{
    std::ifstream in(fileName);

    return in.good();
}


} 


xmrig::TlsGen::~TlsGen()
{
    EVP_PKEY_free(m_pkey);
    X509_free(m_x509);
}


void xmrig::TlsGen::generate(const char *commonName)
{
    if (isFileExist(m_cert) && isFileExist(m_certKey)) {
        return;
    }

    m_pkey = generate_pkey();
    if (!m_pkey) {
        throw std::runtime_error("RSA key generation failed.");
    }

    if (!generate_x509(commonName == nullptr || strlen(commonName) == 0 ? kLocalhost : commonName)) {
        throw std::runtime_error("x509 certificate generation failed.");
    }

    if (!write()) {
        throw std::runtime_error("unable to write certificate to disk.");
    }
}


bool xmrig::TlsGen::generate_x509(const char *commonName)
{
    m_x509 = X509_new();
    if (!m_x509) {
        return false;
    }

    if (!X509_set_pubkey(m_x509, m_pkey)) {
        return false;
    }

    ASN1_INTEGER_set(X509_get_serialNumber(m_x509), 1);
    X509_gmtime_adj(X509_get_notBefore(m_x509), 0);
    X509_gmtime_adj(X509_get_notAfter(m_x509), 315360000L);

    auto name = X509_get_subject_name(m_x509);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const uint8_t *>(commonName), -1, -1, 0);

    X509_set_issuer_name(m_x509, name);

    return X509_sign(m_x509, m_pkey, EVP_sha256());
}


bool xmrig::TlsGen::write()
{
    auto pkey_file = fopen(m_certKey, "wb");
    if (!pkey_file) {
        return false;
    }

    bool ret = PEM_write_PrivateKey(pkey_file, m_pkey, nullptr, nullptr, 0, nullptr, nullptr);
    fclose(pkey_file);

    if (!ret) {
        return false;
    }

    auto x509_file = fopen(m_cert, "wb");
    if (!x509_file) {
        return false;
    }

    ret = PEM_write_X509(x509_file, m_x509);
    fclose(x509_file);

    return ret;
}
