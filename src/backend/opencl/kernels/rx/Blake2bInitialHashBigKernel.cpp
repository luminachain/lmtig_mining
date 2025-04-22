


#include "backend/opencl/kernels/rx/Blake2bInitialHashBigKernel.h"
#include "backend/opencl/wrappers/OclLib.h"


void xmrig::Blake2bInitialHashBigKernel::enqueue(cl_command_queue queue, size_t threads)
{
    const size_t gthreads        = threads;
    static const size_t lthreads = 64;

    enqueueNDRange(queue, 1, nullptr, &gthreads, &lthreads);
}



void xmrig::Blake2bInitialHashBigKernel::setArgs(cl_mem out, cl_mem blockTemplate)
{
    setArg(0, sizeof(cl_mem), &out);
    setArg(1, sizeof(cl_mem), &blockTemplate);
}


void xmrig::Blake2bInitialHashBigKernel::setBlobSize(size_t size)
{
    const uint32_t s = size;

    setArg(2, sizeof(uint32_t), &s);
}


void xmrig::Blake2bInitialHashBigKernel::setNonce(uint32_t nonce, uint32_t nonce_offset)
{
    setArg(3, sizeof(uint32_t), &nonce);
    setArg(4, sizeof(uint32_t), &nonce_offset);
}
