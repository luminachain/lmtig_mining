


#ifdef _MSC_VER
#include <windows.h>
#include <winbase.h> 
#endif
#if defined __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#define VC_GE_2005(version) (version >= 1400)

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "blake2/blake2.h"
#include "blake2/blake2-impl.h"

#include "genkat.h"

#if defined(__clang__)
#if __has_attribute(optnone)
#define NOT_OPTIMIZED __attribute__((optnone))
#endif
#elif defined(__GNUC__)
#define GCC_VERSION                                                            \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION >= 40400
#define NOT_OPTIMIZED __attribute__((optimize("O0")))
#endif
#endif
#ifndef NOT_OPTIMIZED
#define NOT_OPTIMIZED
#endif


void init_block_value(block *b, uint8_t in) { memset(b->v, in, sizeof(b->v)); }

void copy_block(block *dst, const block *src) {
    memcpy(dst->v, src->v, sizeof(uint64_t) * ARGON2_QWORDS_IN_BLOCK);
}

void xor_block(block *dst, const block *src) {
    int i;
    for (i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i) {
        dst->v[i] ^= src->v[i];
    }
}

static void load_block(block *dst, const void *input) {
    unsigned i;
    for (i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i) {
        dst->v[i] = load64((const uint8_t *)input + i * sizeof(dst->v[i]));
    }
}

static void store_block(void *output, const block *src) {
    unsigned i;
    for (i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i) {
        store64((uint8_t *)output + i * sizeof(src->v[i]), src->v[i]);
    }
}



int xmrig_ar2_allocate_memory(const argon2_context *context, argon2_instance_t *instance) {
    size_t blocks = instance->memory_blocks;
    size_t memory_size = blocks * ARGON2_BLOCK_SIZE;

    
    
    if (instance->memory != NULL) {
        return ARGON2_OK;
    }

    
    if (blocks != 0 && memory_size / ARGON2_BLOCK_SIZE != blocks) {
        return ARGON2_MEMORY_ALLOCATION_ERROR;
    }

    
    if (context->allocate_cbk) {
        (context->allocate_cbk)((uint8_t **)&instance->memory, memory_size);
    } else {
        instance->memory = malloc(memory_size);
    }

    if (instance->memory == NULL) {
        return ARGON2_MEMORY_ALLOCATION_ERROR;
    }

    return ARGON2_OK;
}

void xmrig_ar2_free_memory(const argon2_context *context, const argon2_instance_t *instance) {
    size_t memory_size = instance->memory_blocks * ARGON2_BLOCK_SIZE;

    xmrig_ar2_clear_internal_memory(instance->memory, memory_size);

    if (instance->keep_memory) {
        
        return;
    }

    if (context->free_cbk) {
        (context->free_cbk)((uint8_t *)instance->memory, memory_size);
    } else {
        free(instance->memory);
    }
}

void NOT_OPTIMIZED xmrig_ar2_secure_wipe_memory(void *v, size_t n) {
#if defined(_MSC_VER) && VC_GE_2005(_MSC_VER)
    SecureZeroMemory(v, n);
#elif defined memset_s
    memset_s(v, n, 0, n);
#elif defined(__OpenBSD__)
    explicit_bzero(v, n);
#else
    static void *(*const volatile memset_sec)(void *, int, size_t) = &memset;
    memset_sec(v, 0, n);
#endif
}


int FLAG_clear_internal_memory = 0;
void xmrig_ar2_clear_internal_memory(void *v, size_t n) {
    if (FLAG_clear_internal_memory && v) {
        xmrig_ar2_secure_wipe_memory(v, n);
    }
}

void xmrig_ar2_finalize(const argon2_context *context, argon2_instance_t *instance) {
    if (context != NULL && instance != NULL && context->out != NULL) {
        block blockhash;
        uint32_t l;

        copy_block(&blockhash, instance->memory + instance->lane_length - 1);

        
        for (l = 1; l < instance->lanes; ++l) {
            uint32_t last_block_in_lane =
                l * instance->lane_length + (instance->lane_length - 1);
            xor_block(&blockhash, instance->memory + last_block_in_lane);
        }

        
        {
            uint8_t blockhash_bytes[ARGON2_BLOCK_SIZE];
            store_block(blockhash_bytes, &blockhash);
            xmrig_ar2_blake2b_long(context->out, context->outlen, blockhash_bytes, ARGON2_BLOCK_SIZE);
            
            xmrig_ar2_clear_internal_memory(blockhash.v, ARGON2_BLOCK_SIZE);
            xmrig_ar2_clear_internal_memory(blockhash_bytes, ARGON2_BLOCK_SIZE);
        }

        if (instance->print_internals) {
            print_tag(context->out, context->outlen);
        }

        xmrig_ar2_free_memory(context, instance);
    }
}

uint32_t xmrig_ar2_index_alpha(const argon2_instance_t *instance, const argon2_position_t *position, uint32_t pseudo_rand, int same_lane) {
    
    uint32_t reference_area_size;
    uint64_t relative_position;
    uint32_t start_position, absolute_position;

    if (0 == position->pass) {
        
        if (0 == position->slice) {
            
            reference_area_size =
                position->index - 1; 
        } else {
            if (same_lane) {
                
                reference_area_size =
                    position->slice * instance->segment_length +
                    position->index - 1;
            } else {
                reference_area_size =
                    position->slice * instance->segment_length +
                    ((position->index == 0) ? (-1) : 0);
            }
        }
    } else {
        
        if (same_lane) {
            reference_area_size = instance->lane_length -
                                  instance->segment_length + position->index -
                                  1;
        } else {
            reference_area_size = instance->lane_length -
                                  instance->segment_length +
                                  ((position->index == 0) ? (-1) : 0);
        }
    }

    
    relative_position = pseudo_rand;
    relative_position = relative_position * relative_position >> 32;
    relative_position = reference_area_size - 1 -
                        (reference_area_size * relative_position >> 32);

    
    start_position = 0;

    if (0 != position->pass) {
        start_position = (position->slice == ARGON2_SYNC_POINTS - 1)
                             ? 0
                             : (position->slice + 1) * instance->segment_length;
    }

    
    absolute_position = (start_position + relative_position) %
                        instance->lane_length; 
    return absolute_position;
}


static int fill_memory_blocks_st(argon2_instance_t *instance) {
    uint32_t r, s, l;

    for (r = 0; r < instance->passes; ++r) {
        for (s = 0; s < ARGON2_SYNC_POINTS; ++s) {
            for (l = 0; l < instance->lanes; ++l) {
                argon2_position_t position = { r, l, (uint8_t)s, 0 };
                xmrig_ar2_fill_segment(instance, position);
            }
        }

        if (instance->print_internals) {
            internal_kat(instance, r); 
        }
    }
    return ARGON2_OK;
}

int xmrig_ar2_fill_memory_blocks(argon2_instance_t *instance) {
    if (instance == NULL || instance->lanes == 0) {
        return ARGON2_INCORRECT_PARAMETER;
    }

    return fill_memory_blocks_st(instance);
}

int xmrig_ar2_validate_inputs(const argon2_context *context) {
    if (NULL == context) {
        return ARGON2_INCORRECT_PARAMETER;
    }

    
    
    

    
    
    
    

    if (ARGON2_MAX_OUTLEN < context->outlen) {
        return ARGON2_OUTPUT_TOO_LONG;
    }

    
    if (NULL == context->pwd) {
        if (0 != context->pwdlen) {
            return ARGON2_PWD_PTR_MISMATCH;
        }
    }

    if (ARGON2_MIN_PWD_LENGTH > context->pwdlen) {
        return ARGON2_PWD_TOO_SHORT;
    }

    if (ARGON2_MAX_PWD_LENGTH < context->pwdlen) {
        return ARGON2_PWD_TOO_LONG;
    }

    
    if (NULL == context->salt) {
        if (0 != context->saltlen) {
            return ARGON2_SALT_PTR_MISMATCH;
        }
    }

    if (ARGON2_MIN_SALT_LENGTH > context->saltlen) {
        return ARGON2_SALT_TOO_SHORT;
    }

    if (ARGON2_MAX_SALT_LENGTH < context->saltlen) {
        return ARGON2_SALT_TOO_LONG;
    }

    
    if (NULL == context->secret) {
        if (0 != context->secretlen) {
            return ARGON2_SECRET_PTR_MISMATCH;
        }
    } else {
        if (ARGON2_MIN_SECRET > context->secretlen) {
            return ARGON2_SECRET_TOO_SHORT;
        }
        if (ARGON2_MAX_SECRET < context->secretlen) {
            return ARGON2_SECRET_TOO_LONG;
        }
    }

    
    if (NULL == context->ad) {
        if (0 != context->adlen) {
            return ARGON2_AD_PTR_MISMATCH;
        }
    } else {
        if (ARGON2_MIN_AD_LENGTH > context->adlen) {
            return ARGON2_AD_TOO_SHORT;
        }
        if (ARGON2_MAX_AD_LENGTH < context->adlen) {
            return ARGON2_AD_TOO_LONG;
        }
    }

    
    if (ARGON2_MIN_MEMORY > context->m_cost) {
        return ARGON2_MEMORY_TOO_LITTLE;
    }

    if (ARGON2_MAX_MEMORY < context->m_cost) {
        return ARGON2_MEMORY_TOO_MUCH;
    }

    if (context->m_cost < 8 * context->lanes) {
        return ARGON2_MEMORY_TOO_LITTLE;
    }

    
    if (ARGON2_MIN_TIME > context->t_cost) {
        return ARGON2_TIME_TOO_SMALL;
    }

    if (ARGON2_MAX_TIME < context->t_cost) {
        return ARGON2_TIME_TOO_LARGE;
    }

    
    if (ARGON2_MIN_LANES > context->lanes) {
        return ARGON2_LANES_TOO_FEW;
    }

    if (ARGON2_MAX_LANES < context->lanes) {
        return ARGON2_LANES_TOO_MANY;
    }

    
    if (ARGON2_MIN_THREADS > context->threads) {
        return ARGON2_THREADS_TOO_FEW;
    }

    if (ARGON2_MAX_THREADS < context->threads) {
        return ARGON2_THREADS_TOO_MANY;
    }

    if (NULL != context->allocate_cbk && NULL == context->free_cbk) {
        return ARGON2_FREE_MEMORY_CBK_NULL;
    }

    if (NULL == context->allocate_cbk && NULL != context->free_cbk) {
        return ARGON2_ALLOCATE_MEMORY_CBK_NULL;
    }

    return ARGON2_OK;
}

void xmrig_ar2_fill_first_blocks(uint8_t *blockhash, const argon2_instance_t *instance) {
    uint32_t l;
    
    uint8_t blockhash_bytes[ARGON2_BLOCK_SIZE];
    for (l = 0; l < instance->lanes; ++l) {

        store32(blockhash + ARGON2_PREHASH_DIGEST_LENGTH, 0);
        store32(blockhash + ARGON2_PREHASH_DIGEST_LENGTH + 4, l);
        xmrig_ar2_blake2b_long(blockhash_bytes, ARGON2_BLOCK_SIZE, blockhash, ARGON2_PREHASH_SEED_LENGTH);
        load_block(&instance->memory[l * instance->lane_length + 0], blockhash_bytes);

        store32(blockhash + ARGON2_PREHASH_DIGEST_LENGTH, 1);
        xmrig_ar2_blake2b_long(blockhash_bytes, ARGON2_BLOCK_SIZE, blockhash, ARGON2_PREHASH_SEED_LENGTH);
        load_block(&instance->memory[l * instance->lane_length + 1], blockhash_bytes);
    }
    xmrig_ar2_clear_internal_memory(blockhash_bytes, ARGON2_BLOCK_SIZE);
}

void xmrig_ar2_initial_hash(uint8_t *blockhash, argon2_context *context,
                  argon2_type type) {
    blake2b_state BlakeHash;
    uint8_t value[sizeof(uint32_t)];

    if (NULL == context || NULL == blockhash) {
        return;
    }

    xmrig_ar2_blake2b_init(&BlakeHash, ARGON2_PREHASH_DIGEST_LENGTH);

    store32(&value, context->lanes);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    store32(&value, context->outlen);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    store32(&value, context->m_cost);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    store32(&value, context->t_cost);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    store32(&value, context->version);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    store32(&value, (uint32_t)type);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    store32(&value, context->pwdlen);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    if (context->pwd != NULL) {
        xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)context->pwd,
                       context->pwdlen);

        if (context->flags & ARGON2_FLAG_CLEAR_PASSWORD) {
            xmrig_ar2_secure_wipe_memory(context->pwd, context->pwdlen);
            context->pwdlen = 0;
        }
    }

    store32(&value, context->saltlen);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    if (context->salt != NULL) {
        xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)context->salt,
                       context->saltlen);
    }

    store32(&value, context->secretlen);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    if (context->secret != NULL) {
        xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)context->secret, context->secretlen);

        if (context->flags & ARGON2_FLAG_CLEAR_SECRET) {
            xmrig_ar2_secure_wipe_memory(context->secret, context->secretlen);
            context->secretlen = 0;
        }
    }

    store32(&value, context->adlen);
    xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)&value, sizeof(value));

    if (context->ad != NULL) {
        xmrig_ar2_blake2b_update(&BlakeHash, (const uint8_t *)context->ad, context->adlen);
    }

    xmrig_ar2_blake2b_final(&BlakeHash, blockhash, ARGON2_PREHASH_DIGEST_LENGTH);
}

int xmrig_ar2_initialize(argon2_instance_t *instance, argon2_context *context) {
    uint8_t blockhash[ARGON2_PREHASH_SEED_LENGTH];
    int result = ARGON2_OK;

    if (instance == NULL || context == NULL)
        return ARGON2_INCORRECT_PARAMETER;
    instance->context_ptr = context;

    

    result = xmrig_ar2_allocate_memory(context, instance);
    if (result != ARGON2_OK) {
        return result;
    }

    
    
    
    
    xmrig_ar2_initial_hash(blockhash, context, instance->type);
    
    xmrig_ar2_clear_internal_memory(blockhash + ARGON2_PREHASH_DIGEST_LENGTH, ARGON2_PREHASH_SEED_LENGTH - ARGON2_PREHASH_DIGEST_LENGTH);

    if (instance->print_internals) {
        initial_kat(blockhash, context, instance->type);
    }

    
    xmrig_ar2_fill_first_blocks(blockhash, instance);
    
    xmrig_ar2_clear_internal_memory(blockhash, ARGON2_PREHASH_SEED_LENGTH);

    return ARGON2_OK;
}
