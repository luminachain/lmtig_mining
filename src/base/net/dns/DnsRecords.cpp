

#include <uv.h>


#include "base/net/dns/DnsRecords.h"
#include "base/net/dns/Dns.h"


const xmrig::DnsRecord &xmrig::DnsRecords::get(DnsRecord::Type prefered) const
{
    static const DnsRecord defaultRecord;

    if (isEmpty()) {
        return defaultRecord;
    }

    const size_t ipv4 = m_ipv4.size();
    const size_t ipv6 = m_ipv6.size();

    if (ipv6 && (prefered == DnsRecord::AAAA || Dns::config().isIPv6() || !ipv4)) {
        return m_ipv6[ipv6 == 1 ? 0 : static_cast<size_t>(rand()) % ipv6]; 
    }

    if (ipv4) {
        return m_ipv4[ipv4 == 1 ? 0 : static_cast<size_t>(rand()) % ipv4]; 
    }

    return defaultRecord;
}


size_t xmrig::DnsRecords::count(DnsRecord::Type type) const
{
    if (type == DnsRecord::A) {
        return m_ipv4.size();
    }

    if (type == DnsRecord::AAAA) {
        return m_ipv6.size();
    }

    return m_ipv4.size() + m_ipv6.size();
}


void xmrig::DnsRecords::clear()
{
    m_ipv4.clear();
    m_ipv6.clear();
}


void xmrig::DnsRecords::parse(addrinfo *res)
{
    clear();

    addrinfo *ptr = res;
    size_t ipv4   = 0;
    size_t ipv6   = 0;

    while (ptr != nullptr) {
        if (ptr->ai_family == AF_INET) {
            ++ipv4;
        }
        else if (ptr->ai_family == AF_INET6) {
            ++ipv6;
        }

        ptr = ptr->ai_next;
    }

    if (ipv4 == 0 && ipv6 == 0) {
        return;
    }

    m_ipv4.reserve(ipv4);
    m_ipv6.reserve(ipv6);

    ptr = res;
    while (ptr != nullptr) {
        if (ptr->ai_family == AF_INET) {
            m_ipv4.emplace_back(ptr);
        }
        else if (ptr->ai_family == AF_INET6) {
            m_ipv6.emplace_back(ptr);
        }

        ptr = ptr->ai_next;
    }
}
