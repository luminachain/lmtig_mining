


#include "backend/opencl/kernels/Cn0Kernel.h"
#include "backend/opencl/wrappers/OclLib.h"


void xmrig::Cn0Kernel::enqueue(cl_command_queue queue, uint32_t nonce, size_t threads)
{
    const size_t offset[2]          = { nonce, 1 };
    const size_t gthreads[2]        = { threads, 8 };
    static const size_t lthreads[2] = { 8, 8 };

    enqueueNDRange(queue, 2, offset, gthreads, lthreads);
}



void xmrig::Cn0Kernel::setArgs(cl_mem input, int inlen, cl_mem scratchpads, cl_mem states, uint32_t threads)
{
    setArg(0, sizeof(cl_mem), &input);
    setArg(1, sizeof(int), &inlen);
    setArg(2, sizeof(cl_mem), &scratchpads);
    setArg(3, sizeof(cl_mem), &states);
    setArg(4, sizeof(uint32_t), &threads);
}
