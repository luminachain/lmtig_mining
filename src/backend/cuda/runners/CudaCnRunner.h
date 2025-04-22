

#ifndef XMRIG_CUDACNRUNNER_H
#define XMRIG_CUDACNRUNNER_H


#include "backend/cuda/runners/CudaBaseRunner.h"


namespace xmrig {


class CudaCnRunner : public CudaBaseRunner
{
public:
    CudaCnRunner(size_t index, const CudaLaunchData &data);

protected:
    bool run(uint32_t startNonce, uint32_t *rescount, uint32_t *resnonce) override;
};


} 


#endif 
