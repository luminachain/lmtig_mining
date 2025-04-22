#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "encoding.h"
#include "core.h"







#define EQ(x, y) ((((0U - ((unsigned)(x) ^ (unsigned)(y))) >> 8) & 0xFF) ^ 0xFF)
#define GT(x, y) ((((unsigned)(y) - (unsigned)(x)) >> 8) & 0xFF)
#define GE(x, y) (GT(y, x) ^ 0xFF)
#define LT(x, y) GT(y, x)
#define LE(x, y) GE(y, x)


static int b64_byte_to_char(unsigned x) {
    return (LT(x, 26) & (x + 'A')) |
           (GE(x, 26) & LT(x, 52) & (x + ('a' - 26))) |
           (GE(x, 52) & LT(x, 62) & (x + ('0' - 52))) | (EQ(x, 62) & '+') |
           (EQ(x, 63) & '/');
}


static unsigned b64_char_to_byte(int c) {
    unsigned x;

    x = (GE(c, 'A') & LE(c, 'Z') & (c - 'A')) |
        (GE(c, 'a') & LE(c, 'z') & (c - ('a' - 26))) |
        (GE(c, '0') & LE(c, '9') & (c - ('0' - 52))) | (EQ(c, '+') & 62) |
        (EQ(c, '/') & 63);
    return x | (EQ(x, 0) & (EQ(c, 'A') ^ 0xFF));
}


static size_t to_base64(char *dst, size_t dst_len, const void *src,
                        size_t src_len) {
    size_t olen;
    const unsigned char *buf;
    unsigned acc, acc_len;

    olen = (src_len / 3) << 2;
    switch (src_len % 3) {
    case 2:
        olen++;
    
    case 1:
        olen += 2;
        break;
    }
    if (dst_len <= olen) {
        return (size_t)-1;
    }
    acc = 0;
    acc_len = 0;
    buf = (const unsigned char *)src;
    while (src_len-- > 0) {
        acc = (acc << 8) + (*buf++);
        acc_len += 8;
        while (acc_len >= 6) {
            acc_len -= 6;
            *dst++ = (char)b64_byte_to_char((acc >> acc_len) & 0x3F);
        }
    }
    if (acc_len > 0) {
        *dst++ = (char)b64_byte_to_char((acc << (6 - acc_len)) & 0x3F);
    }
    *dst++ = 0;
    return olen;
}


static const char *from_base64(void *dst, size_t *dst_len, const char *src) {
    size_t len;
    unsigned char *buf;
    unsigned acc, acc_len;

    buf = (unsigned char *)dst;
    len = 0;
    acc = 0;
    acc_len = 0;
    for (;;) {
        unsigned d;

        d = b64_char_to_byte(*src);
        if (d == 0xFF) {
            break;
        }
        src++;
        acc = (acc << 6) + d;
        acc_len += 6;
        if (acc_len >= 8) {
            acc_len -= 8;
            if ((len++) >= *dst_len) {
                return NULL;
            }
            *buf++ = (acc >> acc_len) & 0xFF;
        }
    }

    
    if (acc_len > 4 || (acc & (((unsigned)1 << acc_len) - 1)) != 0) {
        return NULL;
    }
    *dst_len = len;
    return src;
}


static const char *decode_decimal(const char *str, unsigned long *v) {
    const char *orig;
    unsigned long acc;

    acc = 0;
    for (orig = str;; str++) {
        int c;

        c = *str;
        if (c < '0' || c > '9') {
            break;
        }
        c -= '0';
        if (acc > (ULONG_MAX / 10)) {
            return NULL;
        }
        acc *= 10;
        if ((unsigned long)c > (ULONG_MAX - acc)) {
            return NULL;
        }
        acc += (unsigned long)c;
    }
    if (str == orig || (*orig == '0' && str != (orig + 1))) {
        return NULL;
    }
    *v = acc;
    return str;
}




int decode_string(argon2_context *ctx, const char *str, argon2_type type) {


#define CC(prefix)                                                             \
    do {                                                                       \
        size_t cc_len = strlen(prefix);                                        \
        if (strncmp(str, prefix, cc_len) != 0) {                               \
            return ARGON2_DECODING_FAIL;                                       \
        }                                                                      \
        str += cc_len;                                                         \
    } while ((void)0, 0)


#define CC_opt(prefix, code)                                                   \
    do {                                                                       \
        size_t cc_len = strlen(prefix);                                        \
        if (strncmp(str, prefix, cc_len) == 0) {                               \
            str += cc_len;                                                     \
            { code; }                                                          \
        }                                                                      \
    } while ((void)0, 0)


#define DECIMAL_U32(x)                                                         \
    do {                                                                       \
        unsigned long dec_x;                                                   \
        str = decode_decimal(str, &dec_x);                                     \
        if (str == NULL || dec_x > UINT32_MAX) {                               \
            return ARGON2_DECODING_FAIL;                                       \
        }                                                                      \
        (x) = (uint32_t)dec_x;                                                           \
    } while ((void)0, 0)


#define BIN(buf, max_len, len)                                                 \
    do {                                                                       \
        size_t bin_len = (max_len);                                            \
        str = from_base64(buf, &bin_len, str);                                 \
        if (str == NULL || bin_len > UINT32_MAX) {                             \
            return ARGON2_DECODING_FAIL;                                       \
        }                                                                      \
        (len) = (uint32_t)bin_len;                                             \
    } while ((void)0, 0)

    size_t maxsaltlen = ctx->saltlen;
    size_t maxoutlen = ctx->outlen;
    int validation_result;
    const char* type_string;

    
    type_string = argon2_type2string(type, 0);
    if (!type_string) {
        return ARGON2_INCORRECT_TYPE;
    }

    CC("$");
    CC(type_string);

    
    ctx->version = ARGON2_VERSION_10;
    CC_opt("$v=", DECIMAL_U32(ctx->version));

    CC("$m=");
    DECIMAL_U32(ctx->m_cost);
    CC(",t=");
    DECIMAL_U32(ctx->t_cost);
    CC(",p=");
    DECIMAL_U32(ctx->lanes);
    ctx->threads = ctx->lanes;

    CC("$");
    BIN(ctx->salt, maxsaltlen, ctx->saltlen);
    CC("$");
    BIN(ctx->out, maxoutlen, ctx->outlen);

    
    ctx->secret = NULL;
    ctx->secretlen = 0;
    ctx->ad = NULL;
    ctx->adlen = 0;
    ctx->allocate_cbk = NULL;
    ctx->free_cbk = NULL;
    ctx->flags = ARGON2_DEFAULT_FLAGS;

    
    validation_result = xmrig_ar2_validate_inputs(ctx);
    if (validation_result != ARGON2_OK) {
        return validation_result;
    }

    
    if (*str == 0) {
        return ARGON2_OK;
    } else {
        return ARGON2_DECODING_FAIL;
    }
#undef CC
#undef CC_opt
#undef DECIMAL_U32
#undef BIN
}

int encode_string(char *dst, size_t dst_len, argon2_context *ctx,
                  argon2_type type) {
#define SS(str)                                                                \
    do {                                                                       \
        size_t pp_len = strlen(str);                                           \
        if (pp_len >= dst_len) {                                               \
            return ARGON2_ENCODING_FAIL;                                       \
        }                                                                      \
        memcpy(dst, str, pp_len + 1);                                          \
        dst += pp_len;                                                         \
        dst_len -= pp_len;                                                     \
    } while ((void)0, 0)

#define SX(x)                                                                  \
    do {                                                                       \
        char tmp[30];                                                          \
        sprintf(tmp, "%lu", (unsigned long)(x));                               \
        SS(tmp);                                                               \
    } while ((void)0, 0)

#define SB(buf, len)                                                           \
    do {                                                                       \
        size_t sb_len = to_base64(dst, dst_len, buf, len);                     \
        if (sb_len == (size_t)-1) {                                            \
            return ARGON2_ENCODING_FAIL;                                       \
        }                                                                      \
        dst += sb_len;                                                         \
        dst_len -= sb_len;                                                     \
    } while ((void)0, 0)

    const char* type_string = argon2_type2string(type, 0);
    int validation_result = xmrig_ar2_validate_inputs(ctx);

    if (!type_string) {
        return ARGON2_ENCODING_FAIL;
    }

    if (validation_result != ARGON2_OK) {
        return validation_result;
    }

    SS("$");
    SS(type_string);

    SS("$v=");
    SX(ctx->version);

    SS("$m=");
    SX(ctx->m_cost);
    SS(",t=");
    SX(ctx->t_cost);
    SS(",p=");
    SX(ctx->lanes);

    SS("$");
    SB(ctx->salt, ctx->saltlen);

    SS("$");
    SB(ctx->out, ctx->outlen);
    return ARGON2_OK;

#undef SS
#undef SX
#undef SB
}

size_t b64len(uint32_t len) {
    size_t olen = ((size_t)len / 3) << 2;

    switch (len % 3) {
    case 2:
        olen++;
    
    case 1:
        olen += 2;
        break;
    }

    return olen;
}

size_t numlen(uint32_t num) {
    size_t len = 1;
    while (num >= 10) {
        ++len;
        num = num / 10;
    }
    return len;
}

