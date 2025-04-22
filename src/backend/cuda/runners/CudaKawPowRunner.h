

#ifndef XMRIG_CUDAKAWPOWRUNNER_H
#define XMRIG_CUDAKAWPOWRUNNER_H


#include "backend/cuda/runners/CudaBaseRunner.h"


namespace xmrig {


class CudaKawPowRunner : public CudaBaseRunner
{
public:
    CudaKawPowRunner(size_t index, const CudaLaunchData &data);

protected:
    bool run(uint32_t startNonce, uint32_t *rescount, uint32_t *resnonce) override;
    bool set(const Job &job, uint8_t *blob) override;
    size_t processedHashes() const override { return intensity() - m_skippedHashes; }
    void jobEarlyNotification(const Job&) override;

private:
    uint8_t* m_jobBlob = nullptr;
    uint32_t m_skippedHashes = 0;
};


} 


#endif 
