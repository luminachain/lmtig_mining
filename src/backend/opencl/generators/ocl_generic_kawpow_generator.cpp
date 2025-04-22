


#include "backend/opencl/OclThreads.h"
#include "backend/opencl/wrappers/OclDevice.h"
#include "base/crypto/Algorithm.h"
#include "crypto/randomx/randomx.h"
#include "crypto/rx/RxAlgo.h"


namespace xmrig {


bool ocl_generic_kawpow_generator(const OclDevice &device, const Algorithm &algorithm, OclThreads &threads)
{
    if (algorithm.family() != Algorithm::KAWPOW) {
        return false;
    }

    bool isNavi = false;

    switch (device.type()) {
    case OclDevice::Navi_10:
    case OclDevice::Navi_12:
    case OclDevice::Navi_14:
    case OclDevice::Navi_21:
        isNavi = true;
        break;

    default:
        break;
    }

    const uint32_t cu_intensity = isNavi ? 524288 : 262144;
    const uint32_t worksize = isNavi ? 128 : 256;
    threads.add(OclThread(device.index(), device.computeUnits() * cu_intensity, worksize, 1));

    return true;
}


} 
