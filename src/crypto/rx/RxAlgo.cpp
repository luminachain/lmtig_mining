

#include "crypto/randomx/randomx.h"
#include "crypto/rx/RxAlgo.h"


xmrig::Algorithm::Id xmrig::RxAlgo::apply(Algorithm::Id algorithm)
{
    randomx_apply_config(*base(algorithm));

    return algorithm;
}


const RandomX_ConfigurationBase *xmrig::RxAlgo::base(Algorithm::Id algorithm)
{
    switch (algorithm) {
    case Algorithm::RX_WOW:
        return &RandomX_WowneroConfig;

    case Algorithm::RX_ARQ:
        return &RandomX_ArqmaConfig;

    case Algorithm::RX_GRAFT:
        return &RandomX_GraftConfig;

    case Algorithm::RX_SFX:
        return &RandomX_SafexConfig;

    case Algorithm::RX_YADA:
        return &RandomX_YadaConfig;

    default:
        break;
    }

    return &RandomX_MoneroConfig;
}


uint32_t xmrig::RxAlgo::version(Algorithm::Id algorithm)
{
    return algorithm == Algorithm::RX_WOW ? 103 : 104;
}


uint32_t xmrig::RxAlgo::programCount(Algorithm::Id algorithm)
{
    return base(algorithm)->ProgramCount;
}


uint32_t xmrig::RxAlgo::programIterations(Algorithm::Id algorithm)
{
    return base(algorithm)->ProgramIterations;
}


uint32_t xmrig::RxAlgo::programSize(Algorithm::Id algorithm)
{
    return base(algorithm)->ProgramSize;
}
