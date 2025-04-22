

#include "KawPow_CalculateDAGKernel.h"
#include "backend/opencl/wrappers/OclLib.h"
#include "crypto/kawpow/KPCache.h"


void xmrig::KawPow_CalculateDAGKernel::enqueue(cl_command_queue queue, size_t threads, size_t workgroup_size)
{
    enqueueNDRange(queue, 1, nullptr, &threads, &workgroup_size);
}


void xmrig::KawPow_CalculateDAGKernel::setArgs(uint32_t start, cl_mem g_light, cl_mem g_dag, uint32_t dag_words, uint32_t light_words)
{
    setArg(0, sizeof(start), &start);
    setArg(1, sizeof(cl_mem), &g_light);
    setArg(2, sizeof(cl_mem), &g_dag);

    const uint32_t isolate = 1;
    setArg(3, sizeof(isolate), &isolate);

    setArg(4, sizeof(dag_words), &dag_words);

    uint32_t light_words4[4];
    KPCache::calculate_fast_mod_data(light_words, light_words4[0], light_words4[1], light_words4[2]);
    light_words4[3] = light_words;

    setArg(5, sizeof(light_words4), light_words4);
}
