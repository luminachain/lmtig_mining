#ifndef ENCODING_H
#define ENCODING_H
#include "3rdparty/argon2.h"

#define ARGON2_MAX_DECODED_LANES UINT32_C(255)
#define ARGON2_MIN_DECODED_SALT_LEN UINT32_C(8)
#define ARGON2_MIN_DECODED_OUT_LEN UINT32_C(12)


int encode_string(char *dst, size_t dst_len, argon2_context *ctx,
                  argon2_type type);


int decode_string(argon2_context *ctx, const char *str, argon2_type type);


size_t b64len(uint32_t len);


size_t numlen(uint32_t num);

#endif
