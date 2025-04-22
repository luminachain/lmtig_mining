


#include "backend/opencl/kernels/rx/HashAesKernel.h"
#include "backend/opencl/wrappers/OclLib.h"


void xmrig::HashAesKernel::enqueue(cl_command_queue queue, size_t threads)
{
    const size_t gthreads        = threads * 4;
    static const size_t lthreads = 64;

    enqueueNDRange(queue, 1, nullptr, &gthreads, &lthreads);
}



void xmrig::HashAesKernel::setArgs(cl_mem input, cl_mem hash, uint32_t hashStrideBytes, uint32_t batch_size)
{
    const uint32_t hashOffsetBytes = 192;

    setArg(0, sizeof(cl_mem), &input);
    setArg(1, sizeof(cl_mem), &hash);
    setArg(2, sizeof(uint32_t), &hashOffsetBytes);
    setArg(3, sizeof(uint32_t), &hashStrideBytes);
    setArg(4, sizeof(uint32_t), &batch_size);
}
