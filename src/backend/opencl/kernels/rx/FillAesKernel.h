

#ifndef XMRIG_FILLAESKERNEL_H
#define XMRIG_FILLAESKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class FillAesKernel : public OclKernel
{
public:
    inline FillAesKernel(cl_program program, const char *name) : OclKernel(program, name) {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem state, cl_mem out, uint32_t batch_size, uint32_t rx_version);
};


} 


#endif 
