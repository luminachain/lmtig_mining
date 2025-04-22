

#ifndef XMRIG_RX_H
#define XMRIG_RX_H


#include <cstdint>
#include <utility>
#include <vector>


#include "crypto/common/HugePagesInfo.h"


namespace xmrig
{


class Algorithm;
class CpuConfig;
class CpuThread;
class IRxListener;
class Job;
class RxConfig;
class RxDataset;


class Rx
{
public:
    static HugePagesInfo hugePages();
    static RxDataset *dataset(const Job &job, uint32_t nodeId);
    static void destroy();
    static void init(IRxListener *listener);
    template<typename T> static bool init(const T &seed, const RxConfig &config, const CpuConfig &cpu);
    template<typename T> static bool isReady(const T &seed);

#   ifdef XMRIG_FEATURE_MSR
    static bool isMSR();
#   else
    static constexpr bool isMSR()   { return false; }
#   endif
};


} 


#endif 
