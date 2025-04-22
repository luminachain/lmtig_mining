

#ifndef XMRIG_HASHAESKERNEL_H
#define XMRIG_HASHAESKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class HashAesKernel : public OclKernel
{
public:
    inline HashAesKernel(cl_program program) : OclKernel(program, "hashAes1Rx4") {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem input, cl_mem hash, uint32_t hashStrideBytes, uint32_t batch_size);
};


} 


#endif 
