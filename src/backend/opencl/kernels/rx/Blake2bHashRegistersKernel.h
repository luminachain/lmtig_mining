

#ifndef XMRIG_BLAKE2BHASHREGISTERSKERNEL_H
#define XMRIG_BLAKE2BHASHREGISTERSKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class Blake2bHashRegistersKernel : public OclKernel
{
public:
    inline Blake2bHashRegistersKernel(cl_program program, const char *name) : OclKernel(program, name) {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem out, cl_mem in, uint32_t inStrideBytes);
};


} 


#endif 
