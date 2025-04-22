


#include "backend/common/GpuWorker.h"
#include "base/tools/Chrono.h"


xmrig::GpuWorker::GpuWorker(size_t id, int64_t affinity, int priority, uint32_t deviceIndex) : Worker(id, affinity, priority),
    m_deviceIndex(deviceIndex)
{
}


void xmrig::GpuWorker::storeStats()
{
    
    const uint32_t index = m_index.load(std::memory_order_relaxed) ^ 1;

    
    m_hashCount[index] = m_count;
    m_timestamp[index] = Chrono::steadyMSecs();

    
    
    m_index.fetch_xor(1, std::memory_order_seq_cst);
}


void xmrig::GpuWorker::hashrateData(uint64_t &hashCount, uint64_t &timeStamp, uint64_t &rawHashes) const
{
    const uint32_t index = m_index.load(std::memory_order_relaxed);

    rawHashes = m_hashrateData.interpolate(timeStamp);
    hashCount = m_hashCount[index];
    timeStamp = m_timestamp[index];
}
