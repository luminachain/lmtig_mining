

#include "base/net/dns/DnsConfig.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"


#include <algorithm>


namespace xmrig {


const char *DnsConfig::kField   = "dns";
const char *DnsConfig::kIPv6    = "ipv6";
const char *DnsConfig::kTTL     = "ttl";


} 


xmrig::DnsConfig::DnsConfig(const rapidjson::Value &value)
{
    m_ipv6  = Json::getBool(value, kIPv6, m_ipv6);
    m_ttl   = std::max(Json::getUint(value, kTTL, m_ttl), 1U);
}


rapidjson::Value xmrig::DnsConfig::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;

    auto &allocator = doc.GetAllocator();
    Value obj(kObjectType);

    obj.AddMember(StringRef(kIPv6), m_ipv6, allocator);
    obj.AddMember(StringRef(kTTL),  m_ttl, allocator);

    return obj;
}
