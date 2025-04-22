

#ifndef XMRIG_OCLRXJITRUNNER_H
#define XMRIG_OCLRXJITRUNNER_H


#include "backend/opencl/runners/OclRxBaseRunner.h"


namespace xmrig {


class RxJitKernel;
class RxRunKernel;


class OclRxJitRunner : public OclRxBaseRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(OclRxJitRunner)

    OclRxJitRunner(size_t index, const OclLaunchData &data);
    ~OclRxJitRunner() override;

protected:
    size_t bufferSize() const override;
    void build() override;
    void execute(uint32_t iteration) override;
    void init() override;

private:
    bool loadAsmProgram();

    cl_mem m_intermediate_programs  = nullptr;
    cl_mem m_programs               = nullptr;
    cl_mem m_registers              = nullptr;
    cl_program m_asmProgram         = nullptr;
    RxJitKernel *m_randomx_jit      = nullptr;
    RxRunKernel *m_randomx_run      = nullptr;
};


} 


#endif 
