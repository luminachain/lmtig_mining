


#include "backend/opencl/kernels/rx/FindSharesKernel.h"
#include "backend/opencl/wrappers/OclLib.h"


void xmrig::FindSharesKernel::enqueue(cl_command_queue queue, size_t threads)
{
    const size_t gthreads        = threads;
    static const size_t lthreads = 64;

    enqueueNDRange(queue, 1, nullptr, &gthreads, &lthreads);
}



void xmrig::FindSharesKernel::setArgs(cl_mem hashes, cl_mem shares)
{
    setArg(0, sizeof(cl_mem), &hashes);
    setArg(3, sizeof(cl_mem), &shares);
}


void xmrig::FindSharesKernel::setTarget(uint64_t target)
{
    setArg(1, sizeof(uint64_t), &target);
}


void xmrig::FindSharesKernel::setNonce(uint32_t nonce)
{
    setArg(2, sizeof(uint32_t), &nonce);
}
