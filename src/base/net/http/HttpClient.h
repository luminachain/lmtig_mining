


#ifndef XMRIG_HTTPCLIENT_H
#define XMRIG_HTTPCLIENT_H


#include "base/kernel/interfaces/IDnsListener.h"
#include "base/kernel/interfaces/ITimerListener.h"
#include "base/net/http/Fetch.h"
#include "base/net/http/HttpContext.h"
#include "base/tools/Object.h"


namespace xmrig {


class DnsRequest;


class HttpClient : public HttpContext, public IDnsListener, public ITimerListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(HttpClient);

    HttpClient(const char *tag, FetchRequest &&req, const std::weak_ptr<IHttpListener> &listener);
    ~HttpClient() override = default;

    inline bool isQuiet() const                 { return m_req.quiet; }
    inline const char *host() const override    { return m_req.host; }
    inline const char *tag() const              { return m_tag; }
    inline uint16_t port() const override       { return m_req.port; }

    bool connect();

protected:
    void onResolved(const DnsRecords &records, int status, const char *error) override;
    void onTimer(const Timer *timer) override;

    virtual void handshake();
    virtual void read(const char *data, size_t size);

protected:
    inline const FetchRequest &req() const  { return m_req; }

private:
    static void onConnect(uv_connect_t *req, int status);

    const char *m_tag;
    FetchRequest m_req;
    std::shared_ptr<DnsRequest> m_dns;
    std::shared_ptr<Timer> m_timer;
};


} 


#endif 

