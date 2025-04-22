

#ifndef XMRIG_INITVMKERNEL_H
#define XMRIG_INITVMKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class InitVmKernel : public OclKernel
{
public:
    inline InitVmKernel(cl_program program) : OclKernel(program, "init_vm") {}

    void enqueue(cl_command_queue queue, size_t threads, uint32_t iteration);
    void setArgs(cl_mem entropy_data, cl_mem vm_states, cl_mem rounding);
};


} 


#endif 
