

#ifndef XMRIG_POOLS_H
#define XMRIG_POOLS_H


#include <vector>


#include "base/net/stratum/Pool.h"


namespace xmrig {


class IJsonReader;
class IStrategy;
class IStrategyListener;


class Pools
{
public:
    static const char *kDonateLevel;
    static const char *kDonateOverProxy;
    static const char *kPools;
    static const char *kRetries;
    static const char *kRetryPause;

    enum ProxyDonate {
        PROXY_DONATE_NONE,
        PROXY_DONATE_AUTO,
        PROXY_DONATE_ALWAYS
    };

    Pools();

#   ifdef XMRIG_FEATURE_BENCHMARK
    inline bool isBenchmark() const                     { return !!m_benchmark; }
#   else
    inline constexpr static bool isBenchmark()          { return false; }
#   endif

    inline const std::vector<Pool> &data() const        { return m_data; }
    inline int retries() const                          { return m_retries; }
    inline int retryPause() const                       { return m_retryPause; }
    inline ProxyDonate proxyDonate() const              { return m_proxyDonate; }

    inline bool operator!=(const Pools &other) const    { return !isEqual(other); }
    inline bool operator==(const Pools &other) const    { return isEqual(other); }

    bool isEqual(const Pools &other) const;
    int donateLevel() const;
    IStrategy *createStrategy(IStrategyListener *listener) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;
    size_t active() const;
    uint32_t benchSize() const;
    void load(const IJsonReader &reader);
    void print() const;
    void toJSON(rapidjson::Value &out, rapidjson::Document &doc) const;

private:
    void setDonateLevel(int level);
    void setProxyDonate(int value);
    void setRetries(int retries);
    void setRetryPause(int retryPause);

    int m_donateLevel;
    int m_retries               = 5;
    int m_retryPause            = 5;
    ProxyDonate m_proxyDonate   = PROXY_DONATE_AUTO;
    std::vector<Pool> m_data;

#   ifdef XMRIG_FEATURE_BENCHMARK
    std::shared_ptr<BenchConfig> m_benchmark;
#   endif
};


} 


#endif 
