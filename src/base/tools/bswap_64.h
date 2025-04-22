

#ifndef XMRIG_BSWAP_64_H
#define XMRIG_BSWAP_64_H

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)
#define bswap_32(x) _byteswap_ulong(x)

#elif defined __GNUC__

#define bswap_64(x) __builtin_bswap64(x)
#define bswap_32(x) __builtin_bswap32(x)

#else

#include <byteswap.h>

#endif

#endif 
