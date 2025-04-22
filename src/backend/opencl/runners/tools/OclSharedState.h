

#ifndef XMRIG_OCLSHAREDSTATE_H
#define XMRIG_OCLSHAREDSTATE_H


#include "backend/opencl/OclLaunchData.h"


namespace xmrig {


class OclSharedState
{
public:
    static OclSharedData &get(uint32_t index);
    static void release();
    static void start(const std::vector<OclLaunchData> &threads, const Job &job);
};


} 


#endif 
