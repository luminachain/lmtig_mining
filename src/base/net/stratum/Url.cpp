

#include "base/net/stratum/Url.h"


#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>


#ifdef _MSC_VER
#   define strncasecmp _strnicmp
#endif


namespace xmrig {

static const char kStratumTcp[]            = "stratum+tcp:
static const char kStratumSsl[]            = "stratum+ssl:
static const char kSOCKS5[]                = "socks5:

#ifdef XMRIG_FEATURE_HTTP
static const char kDaemonHttp[]            = "daemon+http:
static const char kDaemonHttps[]           = "daemon+https:
#endif

} 


xmrig::Url::Url(const char *url)
{
    parse(url);
}


xmrig::Url::Url(const char *host, uint16_t port, bool tls, Scheme scheme) :
    m_tls(tls),
    m_scheme(scheme),
    m_host(host),
    m_port(port)
{
    const size_t size = m_host.size() + 8;
    assert(size > 8);

    char *url = new char[size]();
    snprintf(url, size - 1, "%s:%d", m_host.data(), m_port);

    m_url = url;
}


bool xmrig::Url::isEqual(const Url &other) const
{
    return (m_tls == other.m_tls && m_scheme == other.m_scheme && m_host == other.m_host && m_url == other.m_url && m_port == other.m_port);
}


bool xmrig::Url::parse(const char *url)
{
    if (url == nullptr) {
        return false;
    }

    const char *p    = strstr(url, ":
    const char *base = url;

    if (p) {
        if (strncasecmp(url, kStratumTcp, sizeof(kStratumTcp) - 1) == 0) {
            m_scheme = STRATUM;
            m_tls    = false;
        }
        else if (strncasecmp(url, kStratumSsl, sizeof(kStratumSsl) - 1) == 0) {
            m_scheme = STRATUM;
            m_tls    = true;
        }
        else if (strncasecmp(url, kSOCKS5, sizeof(kSOCKS5) - 1) == 0) {
            m_scheme = SOCKS5;
            m_tls    = false;
        }
#       ifdef XMRIG_FEATURE_HTTP
        else if (strncasecmp(url, kDaemonHttps, sizeof(kDaemonHttps) - 1) == 0) {
            m_scheme = DAEMON;
            m_tls    = true;
        }
        else if (strncasecmp(url, kDaemonHttp, sizeof(kDaemonHttp) - 1) == 0) {
            m_scheme = DAEMON;
            m_tls    = false;
        }
#       endif
        else {
            return false;
        }

        base = p + 3;
    }

    if (!strlen(base) || *base == '/') {
        return false;
    }

    m_url = url;
    if (base[0] == '[') {
        return parseIPv6(base);
    }

    const char *port = strchr(base, ':');
    if (!port) {
        m_host = base;
        return true;
    }

    const auto size = static_cast<size_t>(port++ - base + 1);
    char *host      = new char[size]();
    memcpy(host, base, size - 1);

    m_host = host;
    m_port = static_cast<uint16_t>(strtol(port, nullptr, 10));

    return true;
}


bool xmrig::Url::parseIPv6(const char *addr)
{
    const char *end = strchr(addr, ']');
    if (!end) {
        return false;
    }

    const char *port = strchr(end, ':');
    if (!port) {
        return false;
    }

    const auto size = static_cast<size_t>(end - addr);
    char *host        = new char[size]();
    memcpy(host, addr + 1, size - 1);

    m_host = host;
    m_port = static_cast<uint16_t>(strtol(port + 1, nullptr, 10));

    return true;
}
