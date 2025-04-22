


#include "backend/opencl/runners/tools/OclSharedState.h"
#include "backend/opencl/runners/tools/OclSharedData.h"


#include <cassert>
#include <map>


namespace xmrig {


static std::map<uint32_t, OclSharedData> map;


} 


xmrig::OclSharedData &xmrig::OclSharedState::get(uint32_t index)
{
    return map[index];
}


void xmrig::OclSharedState::release()
{
    for (auto &kv : map) {
        kv.second.release();
    }

    map.clear();
}


void xmrig::OclSharedState::start(const std::vector<OclLaunchData> &threads, const Job &job)
{
    assert(map.empty());

    for (const auto &data : threads) {
        auto &sharedData = map[data.device.index()];

        ++sharedData;

#       ifdef XMRIG_ALGO_RANDOMX
        if (data.algorithm.family() == Algorithm::RANDOM_X) {
            sharedData.createDataset(data.ctx, job, data.thread.isDatasetHost());
        }
#       endif
    }
}
