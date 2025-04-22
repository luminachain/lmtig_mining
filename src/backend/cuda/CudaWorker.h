

#ifndef XMRIG_CUDAWORKER_H
#define XMRIG_CUDAWORKER_H


#include "backend/common/GpuWorker.h"
#include "backend/common/WorkerJob.h"
#include "backend/cuda/CudaLaunchData.h"
#include "base/tools/Object.h"
#include "net/JobResult.h"


namespace xmrig {


class ICudaRunner;


class CudaWorker : public GpuWorker
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(CudaWorker)

    CudaWorker(size_t id, const CudaLaunchData &data);

    ~CudaWorker() override;

    void jobEarlyNotification(const Job &job) override;

    static std::atomic<bool> ready;

protected:
    bool selfTest() override;
    size_t intensity() const override;
    void start() override;

private:
    bool consumeJob();
    void storeStats();

    const Algorithm m_algorithm;
    const Miner *m_miner;
    ICudaRunner *m_runner = nullptr;
    WorkerJob<1> m_job;
};


} 


#endif 
