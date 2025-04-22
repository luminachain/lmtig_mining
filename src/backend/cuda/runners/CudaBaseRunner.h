

#ifndef XMRIG_CUDABASERUNNER_H
#define XMRIG_CUDABASERUNNER_H


#include "backend/cuda/interfaces/ICudaRunner.h"


using nvid_ctx = struct nvid_ctx;


namespace xmrig {


class CudaLaunchData;


class CudaBaseRunner : public ICudaRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(CudaBaseRunner)

    CudaBaseRunner(size_t id, const CudaLaunchData &data);
    ~CudaBaseRunner() override;

protected:
    bool init() override;
    bool set(const Job &job, uint8_t *blob) override;
    size_t intensity() const override;
    size_t roundSize() const override { return intensity(); }
    size_t processedHashes() const override { return intensity(); }
    void jobEarlyNotification(const Job&) override {}

protected:
    bool callWrapper(bool result) const;

    const CudaLaunchData &m_data;
    const size_t m_threadId;
    nvid_ctx *m_ctx     = nullptr;
    uint64_t m_height   = 0;
    uint64_t m_target   = 0;
};


} 


#endif 
