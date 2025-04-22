

#ifndef XMRIG_BLAKE2BINITIALHASHBIGKERNEL_H
#define XMRIG_BLAKE2BINITIALHASHBIGKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class Blake2bInitialHashBigKernel : public OclKernel
{
public:
    inline Blake2bInitialHashBigKernel(cl_program program) : OclKernel(program, "blake2b_initial_hash_big") {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem out, cl_mem blockTemplate);
    void setBlobSize(size_t size);
    void setNonce(uint32_t nonce, uint32_t nonce_offset);
};


} 


#endif 
