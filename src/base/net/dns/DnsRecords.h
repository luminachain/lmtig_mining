

#ifndef XMRIG_DNSRECORDS_H
#define XMRIG_DNSRECORDS_H


#include "base/net/dns/DnsRecord.h"


namespace xmrig {


class DnsRecords
{
public:
    inline bool isEmpty() const       { return m_ipv4.empty() && m_ipv6.empty(); }

    const DnsRecord &get(DnsRecord::Type prefered = DnsRecord::Unknown) const;
    size_t count(DnsRecord::Type type = DnsRecord::Unknown) const;
    void clear();
    void parse(addrinfo *res);

private:
    std::vector<DnsRecord> m_ipv4;
    std::vector<DnsRecord> m_ipv6;
};


} 


#endif 
