


#include "backend/opencl/kernels/rx/InitVmKernel.h"
#include "backend/opencl/wrappers/OclLib.h"


void xmrig::InitVmKernel::enqueue(cl_command_queue queue, size_t threads, uint32_t iteration)
{
    setArg(3, sizeof(uint32_t), &iteration);

    const size_t gthreads        = threads * 8;
    static const size_t lthreads = 32;

    enqueueNDRange(queue, 1, nullptr, &gthreads, &lthreads);
}



void xmrig::InitVmKernel::setArgs(cl_mem entropy_data, cl_mem vm_states, cl_mem rounding)
{
    setArg(0, sizeof(cl_mem), &entropy_data);
    setArg(1, sizeof(cl_mem), &vm_states);
    setArg(2, sizeof(cl_mem), &rounding);
}
