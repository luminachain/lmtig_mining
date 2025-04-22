


#include "backend/opencl/OclLaunchData.h"
#include "backend/common/Tags.h"
#include "backend/opencl/OclConfig.h"


xmrig::OclLaunchData::OclLaunchData(const Miner *miner, const Algorithm &algorithm, const OclConfig &config, const OclPlatform &platform, const OclThread &thread, const OclDevice &device, int64_t affinity) :
    algorithm(algorithm),
    cache(config.isCacheEnabled()),
    affinity(affinity),
    miner(miner),
    device(device),
    platform(platform),
    thread(thread)
{
}


bool xmrig::OclLaunchData::isEqual(const OclLaunchData &other) const
{
    return (other.algorithm == algorithm &&
            other.thread    == thread);
}


const char *xmrig::OclLaunchData::tag()
{
    return ocl_tag();
}
