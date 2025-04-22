

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "sha3.h"
#include "base/crypto/keccak.h"

#define SHA3_ASSERT( x )
#if defined(_MSC_VER)
#define SHA3_TRACE( format, ...)
#define SHA3_TRACE_BUF( format, buf, l, ...)
#else
#define SHA3_TRACE(format, args...)
#define SHA3_TRACE_BUF(format, buf, l, args...)
#endif


#define SHA3_USE_KECCAK_FLAG 0x80000000
#define SHA3_CW(x) ((x) & (~SHA3_USE_KECCAK_FLAG))


#if defined(_MSC_VER)
#define SHA3_CONST(x) x
#else
#define SHA3_CONST(x) x##L
#endif

#define KECCAK_ROUNDS 24





sha3_return_t
sha3_Init(void *priv, unsigned bitSize) {
    sha3_context *ctx = (sha3_context *) priv;
    if( bitSize != 256 && bitSize != 384 && bitSize != 512 )
        return SHA3_RETURN_BAD_PARAMS;
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacityWords = 2 * bitSize / (8 * sizeof(uint64_t));
    return SHA3_RETURN_OK;
}

void
sha3_Init256(void *priv)
{
    sha3_Init(priv, 256);
}

void
sha3_Init384(void *priv)
{
    sha3_Init(priv, 384);
}

void
sha3_Init512(void *priv)
{
    sha3_Init(priv, 512);
}

SHA3_FLAGS
sha3_SetFlags(void *priv, SHA3_FLAGS flags)
{
    sha3_context *ctx = (sha3_context *) priv;
    flags = static_cast<SHA3_FLAGS>(static_cast<int>(flags) & SHA3_FLAGS_KECCAK);
    ctx->capacityWords |= (flags == SHA3_FLAGS_KECCAK ? SHA3_USE_KECCAK_FLAG : 0);
    return flags;
}


void
sha3_Update(void *priv, void const *bufIn, size_t len)
{
    sha3_context *ctx = (sha3_context *) priv;

    
    unsigned old_tail = (8 - ctx->byteIndex) & 7;

    size_t words;
    unsigned tail;
    size_t i;

    const uint8_t *buf = reinterpret_cast<const uint8_t*>(bufIn);

    SHA3_TRACE_BUF("called to update with:", buf, len);

    SHA3_ASSERT(ctx->byteIndex < 8);
    SHA3_ASSERT(ctx->wordIndex < sizeof(ctx->s) / sizeof(ctx->s[0]));

    if(len < old_tail) {        
        SHA3_TRACE("because %d<%d, store it and return", (unsigned)len,
                (unsigned)old_tail);
        
        while (len--)
            ctx->saved |= (uint64_t) (*(buf++)) << ((ctx->byteIndex++) * 8);
        SHA3_ASSERT(ctx->byteIndex < 8);
        return;
    }

    if(old_tail) {              
        SHA3_TRACE("completing one word with %d bytes", (unsigned)old_tail);
        
        len -= old_tail;
        while (old_tail--)
            ctx->saved |= (uint64_t) (*(buf++)) << ((ctx->byteIndex++) * 8);

        
        ctx->s[ctx->wordIndex] ^= ctx->saved;
        SHA3_ASSERT(ctx->byteIndex == 8);
        ctx->byteIndex = 0;
        ctx->saved = 0;
        if(++ctx->wordIndex ==
                (SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx->capacityWords))) {
            xmrig::keccakf(ctx->s, KECCAK_ROUNDS);
            ctx->wordIndex = 0;
        }
    }

    

    SHA3_ASSERT(ctx->byteIndex == 0);

    words = len / sizeof(uint64_t);
    tail = len - words * sizeof(uint64_t);

    SHA3_TRACE("have %d full words to process", (unsigned)words);

    for(i = 0; i < words; i++, buf += sizeof(uint64_t)) {
        const uint64_t t = (uint64_t) (buf[0]) |
                ((uint64_t) (buf[1]) << 8 * 1) |
                ((uint64_t) (buf[2]) << 8 * 2) |
                ((uint64_t) (buf[3]) << 8 * 3) |
                ((uint64_t) (buf[4]) << 8 * 4) |
                ((uint64_t) (buf[5]) << 8 * 5) |
                ((uint64_t) (buf[6]) << 8 * 6) |
                ((uint64_t) (buf[7]) << 8 * 7);
#if defined(__x86_64__ ) || defined(__i386__)
        SHA3_ASSERT(memcmp(&t, buf, 8) == 0);
#endif
        ctx->s[ctx->wordIndex] ^= t;
        if(++ctx->wordIndex ==
                (SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx->capacityWords))) {
            xmrig::keccakf(ctx->s, KECCAK_ROUNDS);
            ctx->wordIndex = 0;
        }
    }

    SHA3_TRACE("have %d bytes left to process, save them", (unsigned)tail);

    
    SHA3_ASSERT(ctx->byteIndex == 0 && tail < 8);
    while (tail--) {
        SHA3_TRACE("Store byte %02x '%c'", *buf, *buf);
        ctx->saved |= (uint64_t) (*(buf++)) << ((ctx->byteIndex++) * 8);
    }
    SHA3_ASSERT(ctx->byteIndex < 8);
    SHA3_TRACE("Have saved=0x%016" PRIx64 " at the end", ctx->saved);
}


void const *
sha3_Finalize(void *priv)
{
    sha3_context *ctx = (sha3_context *) priv;

    SHA3_TRACE("called with %d bytes in the buffer", ctx->byteIndex);

    

    uint64_t t;

    if( ctx->capacityWords & SHA3_USE_KECCAK_FLAG ) {
        
        t = (uint64_t)(((uint64_t) 1) << (ctx->byteIndex * 8));
    }
    else {
        
        t = (uint64_t)(((uint64_t)(0x02 | (1 << 2))) << ((ctx->byteIndex) * 8));
    }

    ctx->s[ctx->wordIndex] ^= ctx->saved ^ t;

    ctx->s[SHA3_KECCAK_SPONGE_WORDS - SHA3_CW(ctx->capacityWords) - 1] ^=
            SHA3_CONST(0x8000000000000000UL);
    xmrig::keccakf(ctx->s, KECCAK_ROUNDS);

    
    {
        unsigned i;
        for(i = 0; i < SHA3_KECCAK_SPONGE_WORDS; i++) {
            const unsigned t1 = (uint32_t) ctx->s[i];
            const unsigned t2 = (uint32_t) ((ctx->s[i] >> 16) >> 16);
            ctx->sb[i * 8 + 0] = (uint8_t) (t1);
            ctx->sb[i * 8 + 1] = (uint8_t) (t1 >> 8);
            ctx->sb[i * 8 + 2] = (uint8_t) (t1 >> 16);
            ctx->sb[i * 8 + 3] = (uint8_t) (t1 >> 24);
            ctx->sb[i * 8 + 4] = (uint8_t) (t2);
            ctx->sb[i * 8 + 5] = (uint8_t) (t2 >> 8);
            ctx->sb[i * 8 + 6] = (uint8_t) (t2 >> 16);
            ctx->sb[i * 8 + 7] = (uint8_t) (t2 >> 24);
        }
    }

    SHA3_TRACE_BUF("Hash: (first 32 bytes)", ctx->sb, 256 / 8);

    return (ctx->sb);
}

extern "C" sha3_return_t sha3_HashBuffer( unsigned bitSize, enum SHA3_FLAGS flags, const void *in, unsigned inBytes, void *out, unsigned outBytes ) {
    sha3_return_t err;
    sha3_context c;

    err = sha3_Init(&c, bitSize);
    if( err != SHA3_RETURN_OK )
        return err;
    if( sha3_SetFlags(&c, flags) != flags ) {
        return SHA3_RETURN_BAD_PARAMS;
    }
    sha3_Update(&c, in, inBytes);
    const void *h = sha3_Finalize(&c);

    if(outBytes > bitSize/8)
        outBytes = bitSize/8;
    memcpy(out, h, outBytes);
    return SHA3_RETURN_OK;
}
