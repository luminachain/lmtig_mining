

#ifndef XMRIG_RXRUNKERNEL_H
#define XMRIG_RXRUNKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class Algorithm;


class RxRunKernel : public OclKernel
{
public:
    inline RxRunKernel(cl_program program) : OclKernel(program, "randomx_run") {}

    void enqueue(cl_command_queue queue, size_t threads, size_t workgroup_size);
    void setArgs(cl_mem dataset, cl_mem scratchpads, cl_mem registers, cl_mem rounding, cl_mem programs, uint32_t batch_size, const Algorithm &algorithm);
};


} 


#endif 
