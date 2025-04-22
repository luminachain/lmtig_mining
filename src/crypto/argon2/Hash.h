

#ifndef XMRIG_ARGON2_HASH_H
#define XMRIG_ARGON2_HASH_H


#include "3rdparty/argon2.h"
#include "base/crypto/Algorithm.h"
#include "crypto/cn/CryptoNight.h"


namespace xmrig { namespace argon2 {


template<Algorithm::Id ALGO>
inline void single_hash(const uint8_t *__restrict__ input, size_t size, uint8_t *__restrict__ output, cryptonight_ctx **__restrict__ ctx, uint64_t)
{
    if (ALGO == Algorithm::AR2_CHUKWA) {
        argon2id_hash_raw_ex(3, 512, 1, input, size, input, 16, output, 32, ctx[0]->memory);
    }
    else if (ALGO == Algorithm::AR2_CHUKWA_V2) {
        argon2id_hash_raw_ex(4, 1024, 1, input, size, input, 16, output, 32, ctx[0]->memory);
    }
    else if (ALGO == Algorithm::AR2_WRKZ) {
        argon2id_hash_raw_ex(4, 256, 1, input, size, input, 16, output, 32, ctx[0]->memory);
    }
}


}} 


#endif 
