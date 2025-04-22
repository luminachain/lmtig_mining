

#ifndef XMRIG_CUDARXRUNNER_H
#define XMRIG_CUDARXRUNNER_H


#include "backend/cuda/runners/CudaBaseRunner.h"


namespace xmrig {


class CudaRxRunner : public CudaBaseRunner
{
public:
    CudaRxRunner(size_t index, const CudaLaunchData &data);

protected:
    inline size_t intensity() const override { return m_intensity; }

    bool run(uint32_t startNonce, uint32_t *rescount, uint32_t *resnonce) override;
    bool set(const Job &job, uint8_t *blob) override;

private:
    bool m_ready             = false;
    const bool m_datasetHost = false;
    size_t m_intensity       = 0;
};


} 


#endif 
