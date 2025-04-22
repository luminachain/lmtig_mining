

#ifndef XMRIG_CN2KERNEL_H
#define XMRIG_CN2KERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class Cn2Kernel : public OclKernel
{
public:
    inline Cn2Kernel(cl_program program) : OclKernel(program, "cn2") {}

    void enqueue(cl_command_queue queue, uint32_t nonce, size_t threads);
    void setArgs(cl_mem scratchpads, cl_mem states, const std::vector<cl_mem> &branches, uint32_t threads);
};


} 


#endif 
