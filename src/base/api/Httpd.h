

#ifndef XMRIG_HTTPD_H
#define XMRIG_HTTPD_H


#include "base/kernel/interfaces/IBaseListener.h"
#include "base/net/http/HttpListener.h"


namespace xmrig {


class Base;
class HttpServer;
class HttpsServer;
class TcpServer;


class Httpd : public IBaseListener, public IHttpListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Httpd)

    explicit Httpd(Base *base);
    ~Httpd() override;

    inline bool isBound() const { return m_server != nullptr; }

    bool start();
    void stop();

protected:
    void onConfigChanged(Config *config, Config *previousConfig) override;
    void onHttpData(const HttpData &data) override;

private:
    int auth(const HttpData &req) const;

    const Base *m_base;
    std::shared_ptr<IHttpListener> m_httpListener;
    TcpServer *m_server     = nullptr;
    uint16_t m_port         = 0;

#   ifdef XMRIG_FEATURE_TLS
    HttpsServer *m_http     = nullptr;
#   else
    HttpServer *m_http      = nullptr;
#   endif
};


} 


#endif 
