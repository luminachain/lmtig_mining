


#include "backend/cpu/CpuLaunchData.h"
#include "backend/common/Tags.h"
#include "backend/cpu/CpuConfig.h"


#include <algorithm>


xmrig::CpuLaunchData::CpuLaunchData(const Miner *miner, const Algorithm &algorithm, const CpuConfig &config, const CpuThread &thread, size_t threads, const std::vector<int64_t>& affinities) :
    algorithm(algorithm),
    assembly(config.assembly()),
    hugePages(config.isHugePages()),
    hwAES(config.isHwAES()),
    yield(config.isYield()),
    priority(config.priority()),
    affinity(thread.affinity()),
    miner(miner),
    threads(threads),
    intensity(std::max<uint32_t>(std::min<uint32_t>(thread.intensity(), algorithm.maxIntensity()), algorithm.minIntensity())),
    affinities(affinities)
{
}


bool xmrig::CpuLaunchData::isEqual(const CpuLaunchData &other) const
{
    return (algorithm.l3()      == other.algorithm.l3()
            && assembly         == other.assembly
            && hugePages        == other.hugePages
            && hwAES            == other.hwAES
            && intensity        == other.intensity
            && priority         == other.priority
            && affinity         == other.affinity
            );
}


xmrig::CnHash::AlgoVariant xmrig::CpuLaunchData::av() const
{
    if (intensity <= 2) {
        return static_cast<CnHash::AlgoVariant>(!hwAES ? (intensity + 2) : intensity);
    }

    return static_cast<CnHash::AlgoVariant>(!hwAES ? (intensity + 5) : (intensity + 2));
}


const char *xmrig::CpuLaunchData::tag()
{
    return cpu_tag();
}
