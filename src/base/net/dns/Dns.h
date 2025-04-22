

#ifndef XMRIG_DNS_H
#define XMRIG_DNS_H


#include "base/net/dns/DnsConfig.h"
#include "base/tools/String.h"


#include <map>
#include <memory>


namespace xmrig {


class DnsConfig;
class DnsRequest;
class IDnsBackend;
class IDnsListener;


class Dns
{
public:
    inline static const DnsConfig &config()             { return m_config; }
    inline static void set(const DnsConfig &config)     { m_config = config; }

    static std::shared_ptr<DnsRequest> resolve(const String &host, IDnsListener *listener, uint64_t ttl = 0);

private:
    static DnsConfig m_config;
    static std::map<String, std::shared_ptr<IDnsBackend> > m_backends;
};


} 


#endif 
