

#ifndef XMRIG_DNSREQUEST_H
#define XMRIG_DNSREQUEST_H


#include "base/tools/Object.h"


#include <cstdint>


namespace xmrig {


class IDnsListener;


class DnsRequest
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(DnsRequest)

    DnsRequest(IDnsListener *listener) : listener(listener) {}
    ~DnsRequest() = default;

    IDnsListener *listener;
};


} 


#endif 
