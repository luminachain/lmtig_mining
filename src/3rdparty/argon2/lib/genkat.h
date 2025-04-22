

#ifndef ARGON2_KAT_H
#define ARGON2_KAT_H

#include "core.h"


void initial_kat(const uint8_t *blockhash, const argon2_context *context,
                 argon2_type type);


void print_tag(const void *out, uint32_t outlen);


void internal_kat(const argon2_instance_t *instance, uint32_t pass);

#endif
