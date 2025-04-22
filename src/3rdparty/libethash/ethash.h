


#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#define ETHASH_REVISION 23
#define ETHASH_DATASET_BYTES_INIT 1073741824U 
#define ETHASH_DATASET_BYTES_GROWTH 8388608U  
#define ETHASH_CACHE_BYTES_INIT 1073741824U 
#define ETHASH_CACHE_BYTES_GROWTH 131072U  
#define ETHASH_EPOCH_LENGTH 30000U
#define ETHASH_MIX_BYTES 128
#define ETHASH_HASH_BYTES 64
#define ETHASH_DATASET_PARENTS 256
#define ETHASH_CACHE_ROUNDS 3
#define ETHASH_ACCESSES 64
#define ETHASH_DAG_MAGIC_NUM_SIZE 8
#define ETHASH_DAG_MAGIC_NUM 0xFEE1DEADBADDCAFE

#ifdef __cplusplus
extern "C" {
#endif


typedef struct ethash_h256 { uint8_t b[32]; } ethash_h256_t;






#define ethash_h256_static_init(...)			\
	{ {__VA_ARGS__} }

struct ethash_light;
typedef struct ethash_light* ethash_light_t;
struct ethash_full;
typedef struct ethash_full* ethash_full_t;
typedef int(*ethash_callback_t)(unsigned);

typedef struct ethash_return_value {
	ethash_h256_t result;
	ethash_h256_t mix_hash;
	bool success;
} ethash_return_value_t;


ethash_light_t ethash_light_new(uint64_t block_number);

bool ethash_compute_cache_nodes(
    void* nodes,
    uint64_t cache_size,
    ethash_h256_t const* seed
);

void ethash_light_delete(ethash_light_t light);

ethash_return_value_t ethash_light_compute(
	ethash_light_t light,
	ethash_h256_t const header_hash,
	uint64_t nonce
);


ethash_full_t ethash_full_new(ethash_light_t light, ethash_callback_t callback);


void ethash_full_delete(ethash_full_t full);

ethash_return_value_t ethash_full_compute(
	ethash_full_t full,
	ethash_h256_t const header_hash,
	uint64_t nonce
);

void const* ethash_full_dag(ethash_full_t full);

uint64_t ethash_full_dag_size(ethash_full_t full);


ethash_h256_t ethash_get_seedhash(uint64_t epoch);


void ethash_keccakf800(uint32_t state[25]);

#ifdef __cplusplus
}
#endif
