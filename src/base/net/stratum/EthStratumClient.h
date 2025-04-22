

#ifndef XMRIG_ETHSTRATUMCLIENT_H
#define XMRIG_ETHSTRATUMCLIENT_H


#include "base/net/stratum/Client.h"


#include <utility>


namespace xmrig {


class EthStratumClient : public Client
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(EthStratumClient)

    EthStratumClient(int id, const char *agent, IClientListener *listener);
    ~EthStratumClient() override = default;

protected:
    int64_t submit(const JobResult &result) override;
    void login() override;
    void onClose() override;

    bool handleResponse(int64_t id, const rapidjson::Value &result, const rapidjson::Value &error) override;
    void parseNotification(const char *method, const rapidjson::Value &params, const rapidjson::Value &error) override;

    void setExtraNonce(const rapidjson::Value &nonce);

#   ifdef XMRIG_ALGO_GHOSTRIDER
    inline void setExtraNonce2Size(uint64_t size)   { m_extraNonce2Size = size; }
#   endif

private:
    static const char *errorMessage(const rapidjson::Value &error);

    void authorize();
    void onAuthorizeResponse(const rapidjson::Value &result, bool success, uint64_t elapsed);
    void onSubscribeResponse(const rapidjson::Value &result, bool success, uint64_t elapsed);
    void subscribe();

    bool m_authorized   = false;
    std::pair<uint64_t, String> m_extraNonce{};

#   ifdef XMRIG_ALGO_GHOSTRIDER
    uint64_t m_extraNonce2Size = 0;
    uint64_t m_nextDifficulty = 0;
    String m_ntime;
#   endif
};


} 


#endif 
