


#include <string>


#include "backend/opencl/kernels/Cn1Kernel.h"
#include "backend/opencl/wrappers/OclLib.h"


xmrig::Cn1Kernel::Cn1Kernel(cl_program program)
    : OclKernel(program, "cn1")
{
}


xmrig::Cn1Kernel::Cn1Kernel(cl_program program, uint64_t height)
    : OclKernel(program, ("cn1_" + std::to_string(height)).c_str())
{

}


void xmrig::Cn1Kernel::enqueue(cl_command_queue queue, uint32_t nonce, size_t threads, size_t worksize)
{
    const size_t offset   = nonce;
    const size_t gthreads = threads;
    const size_t lthreads = worksize;

    enqueueNDRange(queue, 1, &offset, &gthreads, &lthreads);
}



void xmrig::Cn1Kernel::setArgs(cl_mem input, cl_mem scratchpads, cl_mem states, uint32_t threads)
{
    setArg(0, sizeof(cl_mem), &input);
    setArg(1, sizeof(cl_mem), &scratchpads);
    setArg(2, sizeof(cl_mem), &states);
    setArg(3, sizeof(uint32_t), &threads);
}
