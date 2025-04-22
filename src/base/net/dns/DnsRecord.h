

#ifndef XMRIG_DNSRECORD_H
#define XMRIG_DNSRECORD_H


struct addrinfo;
struct sockaddr;


#include "base/tools/String.h"


namespace xmrig {


class DnsRecord
{
public:
    enum Type : uint32_t {
        Unknown,
        A,
        AAAA
    };

    DnsRecord() {}
    DnsRecord(const addrinfo *addr);

    const sockaddr *addr(uint16_t port = 0) const;
    String ip() const;

    inline bool isValid() const     { return m_type != Unknown; }
    inline Type type() const        { return m_type; }

private:
    mutable uint8_t m_data[28]{};
    const Type m_type = Unknown;
};


} 


#endif 
