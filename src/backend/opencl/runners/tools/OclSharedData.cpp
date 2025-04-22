

#include "backend/opencl/runners/tools/OclSharedData.h"
#include "backend/opencl/wrappers/OclLib.h"
#include "base/io/log/Log.h"
#include "base/tools/Chrono.h"
#include "crypto/rx/Rx.h"
#include "crypto/rx/RxDataset.h"


#include <algorithm>
#include <cinttypes>
#include <stdexcept>
#include <thread>


constexpr size_t oneGiB = 1024 * 1024 * 1024;


cl_mem xmrig::OclSharedData::createBuffer(cl_context context, size_t size, size_t &offset, size_t limit)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const size_t pos = offset + (size * m_offset);
    size             = std::max(size * m_threads, oneGiB);

    if (size > limit) {
        return nullptr;
    }

    offset = pos;
    ++m_offset;

    if (!m_buffer) {
        m_buffer = OclLib::createBuffer(context, CL_MEM_READ_WRITE, size);
    }

    return OclLib::retain(m_buffer);
}


uint64_t xmrig::OclSharedData::adjustDelay(size_t )
{
    if (m_threads < 2) {
        return 0;
    }

    const uint64_t t0 = Chrono::steadyMSecs();
    uint64_t delay    = 0;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        const uint64_t dt = t0 - m_timestamp;
        m_timestamp = t0;

        
        
        if ((dt > 0) && (dt < m_threshold * (m_averageRunTime / m_threads))) {
            delay = static_cast<uint64_t>(m_averageRunTime / m_threads - dt);
            m_threshold = 0.75;
        }
    }

    if (delay == 0) {
        return 0;
    }

    if (delay >= 400) {
        delay = 200;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

#   ifdef XMRIG_INTERLEAVE_DEBUG
    LOG_WARN("Thread #%zu was paused for %" PRIu64 " ms to adjust interleaving", id, delay);
#   endif

    return delay;
}


uint64_t xmrig::OclSharedData::resumeDelay(size_t )
{
    if (m_threads < 2) {
        return 0;
    }

    uint64_t delay = 0;

    {
        constexpr const double firstThreadSpeedupCoeff = 1.25;

        std::lock_guard<std::mutex> lock(m_mutex);
        delay = static_cast<uint64_t>(m_resumeCounter * m_averageRunTime / m_threads / firstThreadSpeedupCoeff);
        ++m_resumeCounter;
    }

    if (delay == 0) {
        return 0;
    }

    if (delay > 1000) {
        delay = 1000;
    }

#   ifdef XMRIG_INTERLEAVE_DEBUG
    LOG_WARN("Thread #%zu will be paused for %" PRIu64 " ms to before resuming", id, delay);
#   endif

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    return delay;
}


void xmrig::OclSharedData::release()
{
    OclLib::release(m_buffer);

#   ifdef XMRIG_ALGO_RANDOMX
    OclLib::release(m_dataset);
#   endif
}


void xmrig::OclSharedData::setResumeCounter(uint32_t value)
{
    if (m_threads < 2) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_resumeCounter = value;
}


void xmrig::OclSharedData::setRunTime(uint64_t time)
{
    
    
    
    constexpr double averagingBias = 0.1;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_averageRunTime = m_averageRunTime * (1.0 - averagingBias) + time * averagingBias;
}


#ifdef XMRIG_ALGO_RANDOMX
cl_mem xmrig::OclSharedData::dataset() const
{
    if (!m_dataset) {
        throw std::runtime_error("RandomX dataset is not available");
    }

    return OclLib::retain(m_dataset);
}


void xmrig::OclSharedData::createDataset(cl_context ctx, const Job &job, bool host)
{
    if (m_dataset) {
        return;
    }

    cl_int ret = 0;

    if (host) {
        auto dataset = Rx::dataset(job, 0);

        m_dataset = OclLib::createBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, RxDataset::maxSize(), dataset->raw(), &ret);
    }
    else {
        m_dataset = OclLib::createBuffer(ctx, CL_MEM_READ_ONLY, RxDataset::maxSize(), nullptr, &ret);
    }
}
#endif
