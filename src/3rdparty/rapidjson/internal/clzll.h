













#ifndef RAPIDJSON_CLZLL_H_
#define RAPIDJSON_CLZLL_H_

#include "../rapidjson.h"

#if defined(_MSC_VER) && !defined(UNDER_CE)
#include <intrin.h>
#if defined(_WIN64)
#pragma intrinsic(_BitScanReverse64)
#else
#pragma intrinsic(_BitScanReverse)
#endif
#endif

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

inline uint32_t clzll(uint64_t x) {
    
    
    RAPIDJSON_ASSERT(x != 0);

#if defined(_MSC_VER) && !defined(UNDER_CE)
    unsigned long r = 0;
#if defined(_WIN64)
    _BitScanReverse64(&r, x);
#else
    
    if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
        return 63 - (r + 32);

    
    _BitScanReverse(&r, static_cast<uint32_t>(x & 0xFFFFFFFF));
#endif 

    return 63 - r;
#elif (defined(__GNUC__) && __GNUC__ >= 4) || RAPIDJSON_HAS_BUILTIN(__builtin_clzll)
    
    return static_cast<uint32_t>(__builtin_clzll(x));
#else
    
    uint32_t r = 0;
    while (!(x & (static_cast<uint64_t>(1) << 63))) {
        x <<= 1;
        ++r;
    }

    return r;
#endif 
}

#define RAPIDJSON_CLZLL RAPIDJSON_NAMESPACE::internal::clzll

} 
RAPIDJSON_NAMESPACE_END

#endif 
