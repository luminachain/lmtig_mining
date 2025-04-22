

#ifndef XMRIG_OCLCONFIG_GEN_H
#define XMRIG_OCLCONFIG_GEN_H


#include "backend/common/Threads.h"
#include "backend/opencl/OclThreads.h"


#include <algorithm>


namespace xmrig {


static inline size_t generate(const char *key, Threads<OclThreads> &threads, const Algorithm &algorithm, const std::vector<OclDevice> &devices)
{
    if (threads.isExist(algorithm) || threads.has(key)) {
        return 0;
    }

    return threads.move(key, OclThreads(devices, algorithm));
}


template<Algorithm::Family FAMILY>
static inline size_t generate(Threads<OclThreads> &, const std::vector<OclDevice> &) { return 0; }


template<>
size_t inline generate<Algorithm::CN>(Threads<OclThreads> &threads, const std::vector<OclDevice> &devices)
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
size_t inline generate<Algorithm::CN_LITE>(Threads<OclThreads> &threads, const std::vector<OclDevice> &devices)
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
size_t inline generate<Algorithm::CN_HEAVY>(Threads<OclThreads> &threads, const std::vector<OclDevice> &devices)
{
    return generate(Algorithm::kCN_HEAVY, threads, Algorithm::CN_HEAVY_0, devices);
}
#endif


#ifdef XMRIG_ALGO_CN_PICO
template<>
size_t inline generate<Algorithm::CN_PICO>(Threads<OclThreads> &threads, const std::vector<OclDevice> &devices)
{
    return generate(Algorithm::kCN_PICO, threads, Algorithm::CN_PICO_0, devices);
}
#endif


#ifdef XMRIG_ALGO_CN_FEMTO
template<>
size_t inline generate<Algorithm::CN_FEMTO>(Threads<OclThreads>& threads, const std::vector<OclDevice>& devices)
{
    return generate(Algorithm::kCN_UPX2, threads, Algorithm::CN_UPX2, devices);
}
#endif


#ifdef XMRIG_ALGO_RANDOMX
template<>
size_t inline generate<Algorithm::RANDOM_X>(Threads<OclThreads> &threads, const std::vector<OclDevice> &devices)
{
    size_t count = 0;

    auto rx  = OclThreads(devices, Algorithm::RX_0);
    auto wow = OclThreads(devices, Algorithm::RX_WOW);
    auto arq = OclThreads(devices, Algorithm::RX_ARQ);

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
size_t inline generate<Algorithm::KAWPOW>(Threads<OclThreads>& threads, const std::vector<OclDevice>& devices)
{
    return generate(Algorithm::kKAWPOW, threads, Algorithm::KAWPOW_RVN, devices);
}
#endif


static inline std::vector<OclDevice> filterDevices(const std::vector<OclDevice> &devices, const std::vector<uint32_t> &hints)
{
    std::vector<OclDevice> out;
    out.reserve(std::min(devices.size(), hints.size()));

    for (const auto &device  : devices) {
        auto it = std::find(hints.begin(), hints.end(), device.index());
        if (it != hints.end()) {
            out.emplace_back(device);
        }
    }

    return out;
}


} 


#endif 
