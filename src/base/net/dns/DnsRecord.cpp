


#include <uv.h>


#include "base/net/dns/DnsRecord.h"


xmrig::DnsRecord::DnsRecord(const addrinfo *addr) :
    m_type(addr->ai_family == AF_INET6 ? AAAA : (addr->ai_family == AF_INET ? A : Unknown))
{
    static_assert(sizeof(m_data) >= sizeof(sockaddr_in6), "Not enough storage for IPv6 address.");

    memcpy(m_data, addr->ai_addr, m_type == AAAA ? sizeof(sockaddr_in6) : sizeof(sockaddr_in));
}


const sockaddr *xmrig::DnsRecord::addr(uint16_t port) const
{
    reinterpret_cast<sockaddr_in*>(m_data)->sin_port = htons(port);

    return reinterpret_cast<const sockaddr *>(m_data);
}


xmrig::String xmrig::DnsRecord::ip() const
{
    char *buf = nullptr;

    if (m_type == AAAA) {
        buf = new char[45]();
        uv_ip6_name(reinterpret_cast<const sockaddr_in6*>(m_data), buf, 45);
    }
    else {
        buf = new char[16]();
        uv_ip4_name(reinterpret_cast<const sockaddr_in*>(m_data), buf, 16);
    }

    return buf;
}
