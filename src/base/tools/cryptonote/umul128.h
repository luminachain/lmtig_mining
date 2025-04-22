

#pragma once


#include <cstdint>

#ifdef XMRIG_64_BIT
#   ifdef _MSC_VER
#       include <intrin.h>
#       pragma intrinsic(_umul128)
#       define __umul128 _umul128
#   elif defined __GNUC__
        static inline uint64_t _umul128(uint64_t a, uint64_t b, uint64_t* hi)
        {
            unsigned __int128 r = (unsigned __int128) a * (unsigned __int128) b;
            *hi = r >> 64;
            return (uint64_t) r;
        }
#       define __umul128 _umul128
#   endif
#else
static inline uint64_t __umul128(uint64_t multiplier, uint64_t multiplicand, uint64_t *product_hi) {
    
    
    
    uint64_t a = multiplier >> 32;
    uint64_t b = multiplier & 0xFFFFFFFF;
    uint64_t c = multiplicand >> 32;
    uint64_t d = multiplicand & 0xFFFFFFFF;

    
    uint64_t ad = a * d;
    
    uint64_t bd = b * d;

    uint64_t adbc = ad + (b * c);
    uint64_t adbc_carry = adbc < ad ? 1 : 0;

    
    uint64_t product_lo = bd + (adbc << 32);
    uint64_t product_lo_carry = product_lo < bd ? 1 : 0;
    *product_hi = (a * c) + (adbc >> 32) + (adbc_carry << 32) + product_lo_carry;

    return product_lo;
}
#endif
