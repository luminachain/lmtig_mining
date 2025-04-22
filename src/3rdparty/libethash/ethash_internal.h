#pragma once
#include "endian.h"
#include "ethash.h"
#include <stdio.h>

#define ENABLE_SSE 0

#if defined(_M_X64) && ENABLE_SSE
#include <smmintrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define NODE_WORDS (64/4)
#define MIX_WORDS (ETHASH_MIX_BYTES/4)
#define MIX_NODES (MIX_WORDS / NODE_WORDS)
#include <stdint.h>

typedef union node {
	uint8_t bytes[NODE_WORDS * 4];
	uint32_t words[NODE_WORDS];
	uint64_t double_words[NODE_WORDS / 2];

#if defined(_M_X64) && ENABLE_SSE
	__m128i xmm[NODE_WORDS/4];
#endif

} node;

static inline uint8_t ethash_h256_get(ethash_h256_t const* hash, unsigned int i)
{
	return hash->b[i];
}

static inline void ethash_h256_set(ethash_h256_t* hash, unsigned int i, uint8_t v)
{
	hash->b[i] = v;
}

static inline void ethash_h256_reset(ethash_h256_t* hash)
{
	memset(hash, 0, 32);
}


static inline bool ethash_check_difficulty(
	ethash_h256_t const* hash,
	ethash_h256_t const* boundary
)
{
	
	for (int i = 0; i < 32; i++) {
		if (ethash_h256_get(hash, i) == ethash_h256_get(boundary, i)) {
			continue;
		}
		return ethash_h256_get(hash, i) < ethash_h256_get(boundary, i);
	}
	return true;
}


bool ethash_quick_check_difficulty(
	ethash_h256_t const* header_hash,
	uint64_t const nonce,
	ethash_h256_t const* mix_hash,
	ethash_h256_t const* boundary
);

struct ethash_light {
	void* cache;
	uint64_t cache_size;
	uint64_t block_number;

	
	uint32_t num_parent_nodes;
	uint32_t reciprocal;
	uint32_t increment;
	uint32_t shift;
};


ethash_light_t ethash_light_new_internal(uint64_t cache_size, ethash_h256_t const* seed);


ethash_return_value_t ethash_light_compute_internal(
	ethash_light_t light,
	uint64_t full_size,
	ethash_h256_t const header_hash,
	uint64_t nonce
);

struct ethash_full {
	FILE* file;
	uint64_t file_size;
	node* data;
};


ethash_full_t ethash_full_new_internal(
	char const* dirname,
	ethash_h256_t const seed_hash,
	uint64_t full_size,
	ethash_light_t const light,
	ethash_callback_t callback
);

void ethash_calculate_dag_item(
	node* const ret,
	uint32_t node_index,
	uint32_t num_parents,
	ethash_light_t const cache
);

void ethash_calculate_dag_item_opt(
	node* const ret,
	uint32_t node_index,
	uint32_t num_parents,
	ethash_light_t const cache
);

void ethash_calculate_dag_item4_opt(
	node* ret,
	uint32_t node_index,
	uint32_t num_parents,
	ethash_light_t const cache
);

void ethash_quick_hash(
	ethash_h256_t* return_hash,
	ethash_h256_t const* header_hash,
	const uint64_t nonce,
	ethash_h256_t const* mix_hash
);

uint64_t ethash_get_datasize(uint64_t const block_number);
uint64_t ethash_get_cachesize(uint64_t const block_number);


bool ethash_compute_full_data(
	void* mem,
	uint64_t full_size,
	ethash_light_t const light,
	ethash_callback_t callback
);

#ifdef __cplusplus
}
#endif
