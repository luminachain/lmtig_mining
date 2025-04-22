


#include <cassert>


#include "backend/cpu/Cpu.h"
#include "3rdparty/rapidjson/document.h"


#if defined(XMRIG_FEATURE_HWLOC)
#   include "backend/cpu/platform/HwlocCpuInfo.h"
#else
#   include "backend/cpu/platform/BasicCpuInfo.h"
#endif


static xmrig::ICpuInfo *cpuInfo = nullptr;


xmrig::ICpuInfo *xmrig::Cpu::info()
{
    if (cpuInfo == nullptr) {
#       if defined(XMRIG_FEATURE_HWLOC)
        cpuInfo = new HwlocCpuInfo();
#       else
        cpuInfo = new BasicCpuInfo();
#       endif
    }

    return cpuInfo;
}


rapidjson::Value xmrig::Cpu::toJSON(rapidjson::Document &doc)
{
    return info()->toJSON(doc);
}


void xmrig::Cpu::release()
{
    delete cpuInfo;
    cpuInfo = nullptr;
}
