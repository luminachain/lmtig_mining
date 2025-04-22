

#ifndef XMRIG_KAWPOW_CALCULATEDAGKERNEL_H
#define XMRIG_KAWPOW_CALCULATEDAGKERNEL_H


#include "backend/opencl/wrappers/OclKernel.h"


namespace xmrig {


class KawPow_CalculateDAGKernel : public OclKernel
{
public:
    inline KawPow_CalculateDAGKernel(cl_program program) : OclKernel(program, "ethash_calculate_dag_item") {}

    void enqueue(cl_command_queue queue, size_t threads, size_t workgroup_size);
    void setArgs(uint32_t start, cl_mem g_light, cl_mem g_dag, uint32_t dag_words, uint32_t light_words);
};


} 


#endif 
