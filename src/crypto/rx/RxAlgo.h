

#ifndef XMRIG_RX_ALGO_H
#define XMRIG_RX_ALGO_H


#include <cstddef>
#include <cstdint>


#include "base/crypto/Algorithm.h"


struct RandomX_ConfigurationBase;


namespace xmrig
{


class RxAlgo
{
public:
    static Algorithm::Id apply(Algorithm::Id algorithm);
    static const RandomX_ConfigurationBase *base(Algorithm::Id algorithm);
    static uint32_t programCount(Algorithm::Id algorithm);
    static uint32_t programIterations(Algorithm::Id algorithm);
    static uint32_t programSize(Algorithm::Id algorithm);
    static uint32_t version(Algorithm::Id algorithm);

    static inline Algorithm::Id id(Algorithm::Id algorithm)
    {
        if (algorithm == Algorithm::RX_SFX) {
            return Algorithm::RX_0;
        }

        return algorithm;
    }
};


} 


#endif 
