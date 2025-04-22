

#ifndef XMRIG_CUDALAUNCHDATA_H
#define XMRIG_CUDALAUNCHDATA_H


#include "backend/cuda/CudaThread.h"
#include "base/crypto/Algorithm.h"
#include "crypto/common/Nonce.h"


namespace xmrig {


class CudaDevice;
class Miner;


class CudaLaunchData
{
public:
    CudaLaunchData(const Miner *miner, const Algorithm &algorithm, const CudaThread &thread, const CudaDevice &device);

    bool isEqual(const CudaLaunchData &other) const;

    inline constexpr static Nonce::Backend backend() { return Nonce::CUDA; }

    inline bool operator!=(const CudaLaunchData &other) const    { return !isEqual(other); }
    inline bool operator==(const CudaLaunchData &other) const    { return isEqual(other); }

    static const char *tag();

    const Algorithm algorithm;
    const CudaDevice &device;
    const CudaThread thread;
    const int64_t affinity;
    const Miner *miner;
    const uint32_t benchSize = 0;
};


} 


#endif 
