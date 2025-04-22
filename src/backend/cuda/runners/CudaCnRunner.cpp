


#include "backend/cuda/runners/CudaCnRunner.h"
#include "backend/cuda/wrappers/CudaLib.h"


xmrig::CudaCnRunner::CudaCnRunner(size_t index, const CudaLaunchData &data) : CudaBaseRunner(index, data)
{
}


bool xmrig::CudaCnRunner::run(uint32_t startNonce, uint32_t *rescount, uint32_t *resnonce)
{
    return callWrapper(CudaLib::cnHash(m_ctx, startNonce, m_height, m_target, rescount, resnonce));
}
