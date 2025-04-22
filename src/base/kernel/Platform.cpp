

#include "base/kernel/Platform.h"


#include <cstring>
#include <uv.h>


#ifdef XMRIG_FEATURE_TLS
#   include <openssl/ssl.h>
#   include <openssl/err.h>
#endif


namespace xmrig {

String Platform::m_userAgent;

} 


void xmrig::Platform::init(const char *userAgent)
{
#   ifdef XMRIG_FEATURE_TLS
    SSL_library_init();
    SSL_load_error_strings();

#   if OPENSSL_VERSION_NUMBER < 0x30000000L || defined(LIBRESSL_VERSION_NUMBER)
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
#   endif

    OpenSSL_add_all_digests();
#   endif

    if (userAgent) {
        m_userAgent = userAgent;
    }
    else {
        m_userAgent = createUserAgent();
    }
}
