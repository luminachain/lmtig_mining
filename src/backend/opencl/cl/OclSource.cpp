


#include "backend/opencl/cl/OclSource.h"
#include "backend/opencl/cl/cn/cryptonight_cl.h"
#include "base/crypto/Algorithm.h"


#ifdef XMRIG_ALGO_RANDOMX
#   include "backend/opencl/cl/rx/randomx_cl.h"
#endif

#ifdef XMRIG_ALGO_KAWPOW
#   include "backend/opencl/cl/kawpow/kawpow_cl.h"
#   include "backend/opencl/cl/kawpow/kawpow_dag_cl.h"
#endif


const char *xmrig::OclSource::get(const Algorithm &algorithm)
{
#   ifdef XMRIG_ALGO_RANDOMX
    if (algorithm.family() == Algorithm::RANDOM_X) {
        return randomx_cl;
    }
#   endif

#   ifdef XMRIG_ALGO_KAWPOW
    if (algorithm.family() == Algorithm::KAWPOW) {
        return kawpow_dag_cl;
    }
#   endif

    return cryptonight_cl;
}
