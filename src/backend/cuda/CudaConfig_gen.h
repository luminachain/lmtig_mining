

#ifndef XMRIG_CUDACONFIG_GEN_H
#define XMRIG_CUDACONFIG_GEN_H


#include "backend/common/Threads.h"
#include "backend/cuda/CudaThreads.h"
#include "backend/cuda/wrappers/CudaDevice.h"


#include <algorithm>


namespace xmrig {


static inline size_t generate(const char *key, Threads<CudaThreads> &threads, const Algorithm &algorithm, const std::vector<CudaDevice> &devices)
{
    if (threads.isExist(algorithm) || threads.has(key)) {
        return 0;
    }

    return threads.move(key, CudaThreads(devices, algorithm));
}


template<Algorithm::Family FAMILY>
static inline size_t generate(Threads<CudaThreads> &, const std::vector<CudaDevice> &) { return 0; }


template<>
size_t inline generate<Algorithm::CN>(Threads<CudaThreads> &threads, const std::vector<CudaDevice> &devices)
{
    size_t count = 0;

    count += generate(Algorithm::kCN, threads, Algorithm::CN_1, devices);
    count += generate(Algorithm::kCN_2, threads, Algorithm::CN_2, devices);

    if (!threads.isExist(Algorithm::CN_0)) {
        threads.disable(Algorithm::CN_0);
        count++;
    }

    return count;
}


#ifdef XMRIG_ALGO_CN_LITE
template<>
size_t inline generate<Algorithm::CN_LITE>(Threads<CudaThreads> &threads, const std::vector<CudaDevice> &devices)
{
    size_t count = generate(Algorithm::kCN_LITE, threads, Algorithm::CN_LITE_1, devices);

    if (!threads.isExist(Algorithm::CN_LITE_0)) {
        threads.disable(Algorithm::CN_LITE_0);
        ++count;
    }

    return count;
}
#endif


#ifdef XMRIG_ALGO_CN_HEAVY
template<>
size_t inline generate<Algorithm::CN_HEAVY>(Threads<CudaThreads> &threads, const std::vector<CudaDevice> &devices)
{
    return generate(Algorithm::kCN_HEAVY, threads, Algorithm::CN_HEAVY_0, devices);
}
#endif


#ifdef XMRIG_ALGO_CN_PICO
template<>
size_t inline generate<Algorithm::CN_PICO>(Threads<CudaThreads> &threads, const std::vector<CudaDevice> &devices)
{
    return generate(Algorithm::kCN_PICO, threads, Algorithm::CN_PICO_0, devices);
}
#endif


#ifdef XMRIG_ALGO_CN_FEMTO
template<>
size_t inline generate<Algorithm::CN_FEMTO>(Threads<CudaThreads>& threads, const std::vector<CudaDevice>& devices)
{
    return generate(Algorithm::kCN_UPX2, threads, Algorithm::CN_UPX2, devices);
}
#endif


#ifdef XMRIG_ALGO_RANDOMX
template<>
size_t inline generate<Algorithm::RANDOM_X>(Threads<CudaThreads> &threads, const std::vector<CudaDevice> &devices)
{
    size_t count = 0;

    auto rx  = CudaThreads(devices, Algorithm::RX_0);
    auto wow = CudaThreads(devices, Algorithm::RX_WOW);
    auto arq = CudaThreads(devices, Algorithm::RX_ARQ);

    if (!threads.isExist(Algorithm::RX_WOW) && wow != rx) {
        count += threads.move(Algorithm::kRX_WOW, std::move(wow));
    }

    if (!threads.isExist(Algorithm::RX_ARQ) && arq != rx) {
        count += threads.move(Algorithm::kRX_ARQ, std::move(arq));
    }

    count += threads.move(Algorithm::kRX, std::move(rx));

    return count;
}
#endif


#ifdef XMRIG_ALGO_KAWPOW
template<>
size_t inline generate<Algorithm::KAWPOW>(Threads<CudaThreads> &threads, const std::vector<CudaDevice> &devices)
{
    return generate(Algorithm::kKAWPOW, threads, Algorithm::KAWPOW_RVN, devices);
}
#endif


} 


#endif 
