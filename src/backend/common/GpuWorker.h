

#ifndef XMRIG_GPUWORKER_H
#define XMRIG_GPUWORKER_H


#include <atomic>


#include "backend/common/HashrateInterpolator.h"
#include "backend/common/Worker.h"


namespace xmrig {


class GpuWorker : public Worker
{
public:
    GpuWorker(size_t id, int64_t affinity, int priority, uint32_t m_deviceIndex);

protected:
    inline const VirtualMemory *memory() const override     { return nullptr; }
    inline uint32_t deviceIndex() const                     { return m_deviceIndex; }

    void hashrateData(uint64_t &hashCount, uint64_t &timeStamp, uint64_t &rawHashes) const override;

protected:
    void storeStats();

    const uint32_t m_deviceIndex;
    HashrateInterpolator m_hashrateData;
    std::atomic<uint32_t> m_index   = {};
    uint64_t m_hashCount[2]         = {};
    uint64_t m_timestamp[2]         = {};
};


} 


#endif 
