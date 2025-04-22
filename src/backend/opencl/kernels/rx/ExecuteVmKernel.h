

#ifndef XMRIG_EXECUTEVMKERNEL_H
#define XMRIG_EXECUTEVMKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class ExecuteVmKernel : public OclKernel
{
public:
    inline ExecuteVmKernel(cl_program program) : OclKernel(program, "execute_vm") {}

    void enqueue(cl_command_queue queue, size_t threads, size_t worksize);
    void setArgs(cl_mem vm_states, cl_mem rounding, cl_mem scratchpads, cl_mem dataset_ptr, uint32_t batch_size);
    void setFirst(uint32_t first);
    void setIterations(uint32_t num_iterations);
    void setLast(uint32_t last);
};


} 


#endif 
