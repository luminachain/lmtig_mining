

#ifndef XMRIG_AUTOCLIENT_H
#define XMRIG_AUTOCLIENT_H


#include "base/net/stratum/EthStratumClient.h"


#include <utility>


namespace xmrig {


class AutoClient : public EthStratumClient
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(AutoClient)

    AutoClient(int id, const char *agent, IClientListener *listener);
    ~AutoClient() override = default;

protected:
    inline void login() override    { Client::login(); }

    bool handleResponse(int64_t id, const rapidjson::Value &result, const rapidjson::Value &error) override;
    bool parseLogin(const rapidjson::Value &result, int *code) override;
    int64_t submit(const JobResult &result) override;
    void parseNotification(const char *method, const rapidjson::Value &params, const rapidjson::Value &error) override;

private:
    enum Mode {
        DEFAULT_MODE,
        ETH_MODE
    };

    Mode m_mode = DEFAULT_MODE;
};


} 


#endif 
