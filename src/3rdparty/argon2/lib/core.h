

#ifndef ARGON2_CORE_H
#define ARGON2_CORE_H

#include "3rdparty/argon2.h"

#if defined(_MSC_VER)
#define ALIGN(n) __declspec(align(16))
#elif defined(__GNUC__) || defined(__clang)
#define ALIGN(x) __attribute__((__aligned__(x)))
#else
#define ALIGN(x)
#endif

#define CONST_CAST(x) (x)(uintptr_t)



enum argon2_core_constants {
    
    ARGON2_BLOCK_SIZE = 1024,
    ARGON2_QWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 8,
    ARGON2_OWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 16,

    
    ARGON2_ADDRESSES_IN_BLOCK = 128,

    
    ARGON2_PREHASH_DIGEST_LENGTH = 64,
    ARGON2_PREHASH_SEED_LENGTH = 72
};




typedef struct block_ { uint64_t v[ARGON2_QWORDS_IN_BLOCK]; } block;




void init_block_value(block *b, uint8_t in);


void copy_block(block *dst, const block *src);


void xor_block(block *dst, const block *src);


typedef struct Argon2_instance_t {
    block *memory;          
    uint32_t version;
    uint32_t passes;        
    uint32_t memory_blocks; 
    uint32_t segment_length;
    uint32_t lane_length;
    uint32_t lanes;
    uint32_t threads;
    argon2_type type;
    int print_internals; 
    int keep_memory;
    argon2_context *context_ptr; 
} argon2_instance_t;


typedef struct Argon2_position_t {
    uint32_t pass;
    uint32_t lane;
    uint8_t slice;
    uint32_t index;
} argon2_position_t;


typedef struct Argon2_thread_data {
    argon2_instance_t *instance_ptr;
    argon2_position_t pos;
} argon2_thread_data;




int xmrig_ar2_allocate_memory(const argon2_context *context, argon2_instance_t *instance);


void xmrig_ar2_free_memory(const argon2_context *context, const argon2_instance_t *instance);


void xmrig_ar2_secure_wipe_memory(void *v, size_t n);


ARGON2_PUBLIC void xmrig_ar2_clear_internal_memory(void *v, size_t n);


uint32_t xmrig_ar2_index_alpha(const argon2_instance_t *instance, const argon2_position_t *position, uint32_t pseudo_rand, int same_lane);


int xmrig_ar2_validate_inputs(const argon2_context *context);


void xmrig_ar2_initial_hash(uint8_t *blockhash, argon2_context *context, argon2_type type);


void xmrig_ar2_fill_first_blocks(uint8_t *blockhash, const argon2_instance_t *instance);


int xmrig_ar2_initialize(argon2_instance_t *instance, argon2_context *context);


void xmrig_ar2_finalize(const argon2_context *context, argon2_instance_t *instance);


void xmrig_ar2_fill_segment(const argon2_instance_t *instance, argon2_position_t position);


int xmrig_ar2_fill_memory_blocks(argon2_instance_t *instance);

#endif
