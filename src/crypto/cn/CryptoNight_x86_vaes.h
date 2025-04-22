

#ifndef XMRIG_CRYPTONIGHT_X86_VAES_H
#define XMRIG_CRYPTONIGHT_X86_VAES_H


#include "crypto/cn/CnAlgo.h"


struct cryptonight_ctx;


namespace xmrig {


void cn_explode_scratchpad_vaes(cryptonight_ctx* ctx, size_t memory, bool half_mem);
void cn_explode_scratchpad_vaes_double(cryptonight_ctx* ctx1, cryptonight_ctx* ctx2, size_t memory, bool half_mem);
void cn_implode_scratchpad_vaes(cryptonight_ctx* ctx, size_t memory, bool half_mem);
void cn_implode_scratchpad_vaes_double(cryptonight_ctx* ctx1, cryptonight_ctx* ctx2, size_t memory, bool half_mem);


} 


#endif 
