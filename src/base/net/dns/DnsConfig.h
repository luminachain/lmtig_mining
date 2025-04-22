

#ifndef XMRIG_DNSCONFIG_H
#define XMRIG_DNSCONFIG_H


#include "3rdparty/rapidjson/fwd.h"


namespace xmrig {


class DnsConfig
{
public:
    static const char *kField;
    static const char *kIPv6;
    static const char *kTTL;

    DnsConfig() = default;
    DnsConfig(const rapidjson::Value &value);

    inline bool isIPv6() const  { return m_ipv6; }
    inline uint32_t ttl() const { return m_ttl * 1000U; }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;


private:
    bool m_ipv6     = false;
    uint32_t m_ttl  = 30U;
};


} 


#endif 
