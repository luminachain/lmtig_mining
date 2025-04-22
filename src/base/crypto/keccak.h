


#ifndef XMRIG_KECCAK_H
#define XMRIG_KECCAK_H

#include <cstdint>
#include <cstring>


namespace xmrig {


void keccak(const uint8_t *in, int inlen, uint8_t *md, int mdlen);


inline void keccak(const uint8_t *in, size_t inlen, uint8_t *md)
{
    keccak(in, static_cast<int>(inlen), md, 200);
}


inline void keccak(const char *in, size_t inlen, uint8_t *md)
{
    keccak(reinterpret_cast<const uint8_t *>(in), static_cast<int>(inlen), md, 200);
}


void keccakf(uint64_t st[25], int norounds);

} 

#endif 
