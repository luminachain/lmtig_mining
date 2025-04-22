

#ifndef XMRIG_CNBRANCHKERNEL_H
#define XMRIG_CNBRANCHKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class CnBranchKernel : public OclKernel
{
public:
    CnBranchKernel(size_t index, cl_program program);
    void enqueue(cl_command_queue queue, uint32_t nonce, size_t threads, size_t worksize);
    void setArgs(cl_mem states, cl_mem branch, cl_mem output, uint32_t threads);
    void setTarget(uint64_t target);
};


} 


#endif 
