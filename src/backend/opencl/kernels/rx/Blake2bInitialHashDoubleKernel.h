

#ifndef XMRIG_BLAKE2BINITIALHASHDOUBLEKERNEL_H
#define XMRIG_BLAKE2BINITIALHASHDOUBLEKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class Blake2bInitialHashDoubleKernel : public OclKernel
{
public:
    inline Blake2bInitialHashDoubleKernel(cl_program program) : OclKernel(program, "blake2b_initial_hash_double") {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem out, cl_mem blockTemplate);
    void setBlobSize(size_t size);
    void setNonce(uint32_t nonce);
};


} 


#endif 
