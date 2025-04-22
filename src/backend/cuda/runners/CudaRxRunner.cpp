


#include "backend/cuda/runners/CudaRxRunner.h"
#include "backend/cuda/CudaLaunchData.h"
#include "backend/cuda/wrappers/CudaLib.h"
#include "base/net/stratum/Job.h"
#include "crypto/rx/Rx.h"
#include "crypto/rx/RxDataset.h"


xmrig::CudaRxRunner::CudaRxRunner(size_t index, const CudaLaunchData &data) :
    CudaBaseRunner(index, data),
    m_datasetHost(data.thread.datasetHost() > 0)
{
    m_intensity                   = m_data.thread.threads() * m_data.thread.blocks();
    const size_t scratchpads_size = m_intensity * m_data.algorithm.l3();
    const size_t num_scratchpads  = scratchpads_size / m_data.algorithm.l3();

    if (m_intensity > num_scratchpads) {
        m_intensity = num_scratchpads;
    }

    m_intensity -= m_intensity % 32;
}


bool xmrig::CudaRxRunner::run(uint32_t startNonce, uint32_t *rescount, uint32_t *resnonce)
{
    return callWrapper(CudaLib::rxHash(m_ctx, startNonce, m_target, rescount, resnonce));
}


bool xmrig::CudaRxRunner::set(const Job &job, uint8_t *blob)
{
    const bool rc = CudaBaseRunner::set(job, blob);
    if (!rc || m_ready) {
        return rc;
    }

    auto dataset = Rx::dataset(job, 0);
    m_ready = callWrapper(CudaLib::rxPrepare(m_ctx, dataset->raw(), dataset->size(false), m_datasetHost, m_intensity));

    return m_ready;
}
