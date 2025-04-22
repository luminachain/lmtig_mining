

#ifndef XMRIG_URL_H
#define XMRIG_URL_H


#include "base/tools/String.h"


namespace xmrig {


class Url
{
public:
    enum Scheme {
        UNSPECIFIED,
        STRATUM,
        DAEMON,
        SOCKS5
    };

    Url() = default;
    Url(const char *url);
    Url(const char *host, uint16_t port, bool tls = false, Scheme scheme = UNSPECIFIED);

    inline bool isTLS() const                           { return m_tls; }
    inline bool isValid() const                         { return !m_host.isNull() && m_port > 0; }
    inline const String &host() const                   { return m_host; }
    inline const String &url() const                    { return m_url; }
    inline Scheme scheme() const                        { return m_scheme; }
    inline uint16_t port() const                        { return m_port; }

    inline bool operator!=(const Url &other) const      { return !isEqual(other); }
    inline bool operator==(const Url &other) const      { return isEqual(other); }

    bool isEqual(const Url &other) const;

protected:
    bool parse(const char *url);
    bool parseIPv6(const char *addr);

    bool m_tls      = false;
    Scheme m_scheme = UNSPECIFIED;
    String m_host;
    String m_url;
    uint16_t m_port = 3333;
};


} 


#endif 
