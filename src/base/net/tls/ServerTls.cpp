


#include "base/net/tls/ServerTls.h"


#include <algorithm>
#include <cassert>
#include <cstring>
#include <openssl/ssl.h>


xmrig::ServerTls::ServerTls(SSL_CTX *ctx) :
    m_ctx(ctx)
{
}


xmrig::ServerTls::~ServerTls()
{
    if (m_ssl) {
        SSL_free(m_ssl);
    }
}


bool xmrig::ServerTls::isHTTP(const char *data, size_t size)
{
    assert(size > 0);

    static const char test[6] = "GET /";

    return size > 0 && memcmp(data, test, std::min(size, sizeof(test) - 1)) == 0;
}


bool xmrig::ServerTls::isTLS(const char *data, size_t size)
{
    assert(size > 0);

    static const uint8_t test[3] = { 0x16, 0x03, 0x01 };

    return size > 0 && memcmp(data, test, std::min(size, sizeof(test))) == 0;
}


bool xmrig::ServerTls::send(const char *data, size_t size)
{
    SSL_write(m_ssl, data, size);

    return write(m_write);
}


void xmrig::ServerTls::read(const char *data, size_t size)
{
    if (!m_ssl) {
        m_ssl = SSL_new(m_ctx);

        m_write = BIO_new(BIO_s_mem());
        m_read  = BIO_new(BIO_s_mem());

        SSL_set_accept_state(m_ssl);
        SSL_set_bio(m_ssl, m_read, m_write);
    }


    BIO_write(m_read, data, size);

    if (!SSL_is_init_finished(m_ssl)) {
        const int rc = SSL_do_handshake(m_ssl);

        if (rc < 0 && SSL_get_error(m_ssl, rc) == SSL_ERROR_WANT_READ) {
            write(m_write);
        } else if (rc == 1) {
            write(m_write);

            m_ready = true;
            read();
        }
        else {
            shutdown();
        }

      return;
    }

    read();
}


void xmrig::ServerTls::read()
{
    static char buf[16384]{};

    int bytes_read = 0;
    while ((bytes_read = SSL_read(m_ssl, buf, sizeof(buf))) > 0) {
        parse(buf, bytes_read);
    }
}
