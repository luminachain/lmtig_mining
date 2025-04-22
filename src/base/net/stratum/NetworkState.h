

#ifndef XMRIG_NETWORKSTATE_H
#define XMRIG_NETWORKSTATE_H


#include "base/crypto/Algorithm.h"
#include "base/net/stratum/strategies/StrategyProxy.h"
#include "base/tools/String.h"


#include <array>
#include <string>
#include <vector>


namespace xmrig {


class NetworkState : public StrategyProxy
{
public:
    NetworkState(IStrategyListener *listener);

    inline const Algorithm &algorithm() const   { return m_algorithm; }
    inline uint64_t accepted() const            { return m_accepted; }
    inline uint64_t rejected() const            { return m_rejected; }

#   ifdef XMRIG_FEATURE_API
    rapidjson::Value getConnection(rapidjson::Document &doc, int version) const;
    rapidjson::Value getResults(rapidjson::Document &doc, int version) const;
#   endif

    void printConnection() const;
    void printResults() const;

    static const char *scaleDiff(uint64_t &diff);
    static std::string humanDiff(uint64_t diff);

protected:
    void onActive(IStrategy *strategy, IClient *client) override;
    void onJob(IStrategy *strategy, IClient *client, const Job &job, const rapidjson::Value &params) override;
    void onPause(IStrategy *strategy) override;
    void onResultAccepted(IStrategy *strategy, IClient *client, const SubmitResult &result, const char *error) override;

private:
    uint32_t latency() const;
    uint64_t avgTime() const;
    uint64_t connectionTime() const;
    void add(const SubmitResult &result, const char *error);
    void stop();

    Algorithm m_algorithm;
    bool m_active               = false;
    char m_pool[256]{};
    std::array<uint64_t, 10> m_topDiff { { } };
    std::vector<uint16_t> m_latency;
    String m_fingerprint;
    String m_ip;
    String m_tls;
    uint64_t m_accepted         = 0;
    uint64_t m_connectionTime   = 0;
    uint64_t m_diff             = 0;
    uint64_t m_failures         = 0;
    uint64_t m_hashes           = 0;
    uint64_t m_rejected         = 0;
};


} 


#endif 
