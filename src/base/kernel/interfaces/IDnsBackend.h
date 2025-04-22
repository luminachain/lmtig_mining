

#ifndef XMRIG_IDNSBACKEND_H
#define XMRIG_IDNSBACKEND_H


#include "base/tools/Object.h"


#include <memory>


namespace xmrig {


class DnsRecords;
class DnsRequest;
class IDnsListener;
class String;


class IDnsBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE(IDnsBackend)

    IDnsBackend()           = default;
    virtual ~IDnsBackend()  = default;

    virtual const DnsRecords &records() const                                                               = 0;
    virtual std::shared_ptr<DnsRequest> resolve(const String &host, IDnsListener *listener, uint64_t ttl)   = 0;
};


} 


#endif 
