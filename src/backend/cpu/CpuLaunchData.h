

#ifndef XMRIG_CPULAUNCHDATA_H
#define XMRIG_CPULAUNCHDATA_H


#include "base/crypto/Algorithm.h"
#include "crypto/cn/CnHash.h"
#include "crypto/common/Assembly.h"
#include "crypto/common/Nonce.h"


namespace xmrig {


class CpuConfig;
class CpuThread;
class Miner;


class CpuLaunchData
{
public:
    CpuLaunchData(const Miner *miner, const Algorithm &algorithm, const CpuConfig &config, const CpuThread &thread, size_t threads, const std::vector<int64_t>& affinities);

    bool isEqual(const CpuLaunchData &other) const;
    CnHash::AlgoVariant av() const;

    inline constexpr static Nonce::Backend backend()            { return Nonce::CPU; }

    inline bool operator!=(const CpuLaunchData &other) const    { return !isEqual(other); }
    inline bool operator==(const CpuLaunchData &other) const    { return isEqual(other); }

    static const char *tag();

    const Algorithm algorithm;
    const Assembly assembly;
    const bool hugePages;
    const bool hwAES;
    const bool yield;
    const int priority;
    const int64_t affinity;
    const Miner *miner;
    const size_t threads;
    const uint32_t intensity;
    const std::vector<int64_t> affinities;
};


} 


#endif 
