

#ifndef XMRIG_CN_CTX_H
#define XMRIG_CN_CTX_H


#include <cstddef>
#include <cstdint>


struct cryptonight_ctx;


namespace xmrig
{


class CnCtx
{
public:
    static void create(cryptonight_ctx **ctx, uint8_t *memory, size_t size, size_t count);
    static void release(cryptonight_ctx **ctx, size_t count);
};


} 


#endif 
