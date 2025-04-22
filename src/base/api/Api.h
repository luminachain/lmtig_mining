

#ifndef XMRIG_API_H
#define XMRIG_API_H


#include <vector>


#include "base/kernel/interfaces/IBaseListener.h"
#include "base/tools/String.h"


namespace xmrig {


class Base;
class Httpd;
class HttpData;
class IApiListener;
class IApiRequest;
class String;


class Api : public IBaseListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Api)

    explicit Api(Base *base);
    ~Api() override;

    inline const char *id() const                   { return m_id; }
    inline const char *workerId() const             { return m_workerId; }
    inline void addListener(IApiListener *listener) { m_listeners.push_back(listener); }

    void request(const HttpData &req);
    void start();
    void stop();
    void tick();

protected:
    void onConfigChanged(Config *config, Config *previousConfig) override;

private:
    void exec(IApiRequest &request);
    void genId(const String &id);
    void genWorkerId(const String &id);

    Base *m_base;
    char m_id[32]{};
    const uint64_t m_timestamp;
    Httpd *m_httpd  = nullptr;
    std::vector<IApiListener *> m_listeners;
    String m_workerId;
    uint8_t m_ticks = 0;
};


} 


#endif 
