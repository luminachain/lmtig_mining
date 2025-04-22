


#include "base/net/dns/Dns.h"
#include "base/net/dns/DnsUvBackend.h"


namespace xmrig {


DnsConfig Dns::m_config;
std::map<String, std::shared_ptr<IDnsBackend> > Dns::m_backends;


} 


std::shared_ptr<xmrig::DnsRequest> xmrig::Dns::resolve(const String &host, IDnsListener *listener, uint64_t ttl)
{
    if (m_backends.find(host) == m_backends.end()) {
        m_backends.insert({ host, std::make_shared<DnsUvBackend>() });
    }

    return m_backends.at(host)->resolve(host, listener, ttl == 0 ? m_config.ttl() : ttl);
}
