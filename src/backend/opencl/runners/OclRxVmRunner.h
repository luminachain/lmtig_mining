

#ifndef XMRIG_OCLRXVMRUNNER_H
#define XMRIG_OCLRXVMRUNNER_H


#include "backend/opencl/runners/OclRxBaseRunner.h"


namespace xmrig {


class ExecuteVmKernel;
class InitVmKernel;


class OclRxVmRunner : public OclRxBaseRunner
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(OclRxVmRunner)

    OclRxVmRunner(size_t index, const OclLaunchData &data);
    ~OclRxVmRunner() override;

protected:
    size_t bufferSize() const override;
    void build() override;
    void execute(uint32_t iteration) override;
    void init() override;

private:
    cl_mem m_vm_states              = nullptr;
    ExecuteVmKernel *m_execute_vm   = nullptr;
    InitVmKernel *m_init_vm         = nullptr;
};


} 


#endif 
