


#include "backend/opencl/kernels/rx/RxRunKernel.h"
#include "backend/opencl/wrappers/OclLib.h"
#include "base/crypto/Algorithm.h"
#include "crypto/randomx/randomx.h"
#include "crypto/rx/RxAlgo.h"


void xmrig::RxRunKernel::enqueue(cl_command_queue queue, size_t threads, size_t workgroup_size)
{
    const size_t gthreads        = threads * workgroup_size;
    enqueueNDRange(queue, 1, nullptr, &gthreads, &workgroup_size);
}


void xmrig::RxRunKernel::setArgs(cl_mem dataset, cl_mem scratchpads, cl_mem registers, cl_mem rounding, cl_mem programs, uint32_t batch_size, const Algorithm &algorithm)
{
    setArg(0, sizeof(cl_mem), &dataset);
    setArg(1, sizeof(cl_mem), &scratchpads);
    setArg(2, sizeof(cl_mem), &registers);
    setArg(3, sizeof(cl_mem), &rounding);
    setArg(4, sizeof(cl_mem), &programs);
    setArg(5, sizeof(uint32_t), &batch_size);

    auto PowerOf2 = [](size_t N)
    {
        uint32_t result = 0;
        while (N > 1) {
            ++result;
            N >>= 1;
        }

        return result;
    };

    const auto *rx_conf = RxAlgo::base(algorithm);
    const uint32_t rx_parameters =
                    (PowerOf2(rx_conf->ScratchpadL1_Size) << 0) |
                    (PowerOf2(rx_conf->ScratchpadL2_Size) << 5) |
                    (PowerOf2(rx_conf->ScratchpadL3_Size) << 10) |
                    (PowerOf2(rx_conf->ProgramIterations) << 15);

    setArg(6, sizeof(uint32_t), &rx_parameters);
}
