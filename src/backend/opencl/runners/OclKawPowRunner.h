

#ifndef XMRIG_OCLKAWPOWRUNNER_H
#define XMRIG_OCLKAWPOWRUNNER_H


#include "backend/opencl/runners/OclBaseRunner.h"
#include "crypto/kawpow/KPCache.h"

#include <mutex>

namespace xmrig {


class KawPow_CalculateDAGKernel;


class OclKawPowRunner : public OclBaseRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(OclKawPowRunner)

    OclKawPowRunner(size_t index, const OclLaunchData &data);
    ~OclKawPowRunner() override;

protected:
    void run(uint32_t nonce, uint32_t nonce_offset, uint32_t *hashOutput) override;
    void set(const Job &job, uint8_t *blob) override;
    void build() override;
    void init() override;
    void jobEarlyNotification(const Job& job) override;
    uint32_t processedHashes() const override { return m_intensity - m_skippedHashes; }

private:
    uint8_t* m_blob = nullptr;
    uint32_t m_skippedHashes = 0;

    uint32_t m_blockHeight = 0;
    uint32_t m_epoch = 0xFFFFFFFFUL;

    cl_mem m_lightCache = nullptr;
    size_t m_lightCacheSize = 0;
    size_t m_lightCacheCapacity = 0;

    cl_mem m_dag = nullptr;
    size_t m_dagCapacity = 0;

    KawPow_CalculateDAGKernel* m_calculateDagKernel = nullptr;

    cl_kernel m_searchKernel = nullptr;

    size_t m_workGroupSize = 256;
    size_t m_dagWorkGroupSize = 64;

    cl_command_queue m_controlQueue = nullptr;
    cl_mem m_stop = nullptr;
};


} 


#endif 
