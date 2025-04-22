

#ifndef XMRIG_FINDSHARESKERNEL_H
#define XMRIG_FINDSHARESKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class FindSharesKernel : public OclKernel
{
public:
    inline FindSharesKernel(cl_program program) : OclKernel(program, "find_shares") {}

    void enqueue(cl_command_queue queue, size_t threads);
    void setArgs(cl_mem hashes, cl_mem shares);
    void setTarget(uint64_t target);
    void setNonce(uint32_t nonce);
};


} 


#endif 
