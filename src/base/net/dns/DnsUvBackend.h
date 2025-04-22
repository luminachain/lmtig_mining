

#ifndef XMRIG_DNSUVBACKEND_H
#define XMRIG_DNSUVBACKEND_H


#include "base/kernel/interfaces/IDnsBackend.h"
#include "base/net/dns/DnsRecords.h"
#include "base/net/tools/Storage.h"


#include <queue>


using uv_getaddrinfo_t = struct uv_getaddrinfo_s;


namespace xmrig {


class DnsUvBackend : public IDnsBackend
{
public:
    XMRIG_DISABLE_COPY_MOVE(DnsUvBackend)

    DnsUvBackend();
    ~DnsUvBackend() override;

protected:
    inline const DnsRecords &records() const override   { return m_records; }

    std::shared_ptr<DnsRequest> resolve(const String &host, IDnsListener *listener, uint64_t ttl) override;

private:
    bool resolve(const String &host);
    void done();
    void onResolved(int status, addrinfo *res);

    static void onResolved(uv_getaddrinfo_t *req, int status, addrinfo *res);

    DnsRecords m_records;
    int m_status            = 0;
    std::queue<std::weak_ptr<DnsRequest> > m_queue;
    std::shared_ptr<uv_getaddrinfo_t> m_req;
    uint64_t m_ts           = 0;
    uintptr_t m_key;

    static Storage<DnsUvBackend>& getStorage();
    void releaseStorage();
};


} 


#endif 
