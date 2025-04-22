

#ifndef XMRIG_KP_HASH_H
#define XMRIG_KP_HASH_H


#include <cstdint>


namespace xmrig
{


class KPCache;


class KPHash
{
public:
    static constexpr uint32_t EPOCH_LENGTH  = 7500;
    static constexpr uint32_t PERIOD_LENGTH = 3;
    static constexpr int CNT_CACHE          = 11;
    static constexpr int CNT_MATH           = 18;
    static constexpr uint32_t REGS          = 32;
    static constexpr uint32_t LANES         = 16;

    static void calculate(const KPCache& light_cache, uint32_t block_height, const uint8_t (&header_hash)[32], uint64_t nonce, uint32_t (&output)[8], uint32_t (&mix_hash)[8]);
};


} 


#endif 
