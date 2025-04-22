

#ifndef XMRIG_CN1KERNEL_H
#define XMRIG_CN1KERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class Cn1Kernel : public OclKernel
{
public:
    Cn1Kernel(cl_program program);
    Cn1Kernel(cl_program program, uint64_t height);

    void enqueue(cl_command_queue queue, uint32_t nonce, size_t threads, size_t worksize);
    void setArgs(cl_mem input, cl_mem scratchpads, cl_mem states, uint32_t threads);
};


} 


#endif 
