

#ifndef XMRIG_SOCKS5_H
#define XMRIG_SOCKS5_H


#include "base/net/stratum/Client.h"


namespace xmrig {


class Client::Socks5
{
public:
    Socks5(Client *client);

    inline bool isReady() const     { return m_state == Ready; }

    bool read(const char *data, size_t size);
    void handshake();

private:
    enum State {
        Created,
        SentInitialHandshake,
        SentFinalHandshake,
        Ready
    };

    static bool isIPv4(const String &host, sockaddr_storage *addr);
    static bool isIPv6(const String &host, sockaddr_storage *addr);

    void connect();

    Client *m_client;
    size_t m_nextSize   = 0;
    State m_state       = Created;
};


} 


#endif 
