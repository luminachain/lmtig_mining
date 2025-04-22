


#include "backend/cuda/CudaLaunchData.h"
#include "backend/common/Tags.h"


xmrig::CudaLaunchData::CudaLaunchData(const Miner *miner, const Algorithm &algorithm, const CudaThread &thread, const CudaDevice &device) :
    algorithm(algorithm),
    device(device),
    thread(thread),
    affinity(thread.affinity()),
    miner(miner)
{
}


bool xmrig::CudaLaunchData::isEqual(const CudaLaunchData &other) const
{
    return (other.algorithm.family() == algorithm.family() &&
            other.algorithm.l3()     == algorithm.l3() &&
            other.thread             == thread);
}


const char *xmrig::CudaLaunchData::tag()
{
    return cuda_tag();
}
