

#ifndef XMRIG_OCLWORKER_H
#define XMRIG_OCLWORKER_H


#include "backend/common/GpuWorker.h"
#include "backend/common/WorkerJob.h"
#include "backend/opencl/OclLaunchData.h"
#include "base/tools/Object.h"
#include "net/JobResult.h"


namespace xmrig {


class IOclRunner;
class Job;


class OclWorker : public GpuWorker
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(OclWorker)

    OclWorker(size_t id, const OclLaunchData &data);

    ~OclWorker() override;

    void jobEarlyNotification(const Job &job) override;

    static std::atomic<bool> ready;

protected:
    bool selfTest() override;
    size_t intensity() const override;
    void start() override;

private:
    bool consumeJob();
    void storeStats(uint64_t ts);

    const Algorithm m_algorithm;
    const Miner *m_miner;
    IOclRunner *m_runner = nullptr;
    OclSharedData &m_sharedData;
    WorkerJob<1> m_job;
};


} 


#endif 
