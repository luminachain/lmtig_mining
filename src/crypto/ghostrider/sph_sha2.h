


#ifndef SPH_SHA2_H__
#define SPH_SHA2_H__

#include <stddef.h>
#include <stdint.h>
#include "sph_types.h"


#define SPH_SIZE_sha224   224


#define SPH_SIZE_sha256   256


typedef struct {
#ifndef DOXYGEN_IGNORE
	unsigned char buf[64];    
	sph_u32 val[8];
#if SPH_64
	sph_u64 count;
#else
	sph_u32 count_high, count_low;
#endif
#endif
} sph_sha224_context;


typedef sph_sha224_context sph_sha256_context;


void sph_sha224_init(void *cc);


void sph_sha224(void *cc, const void *data, size_t len);


void sph_sha224_close(void *cc, void *dst);


void sph_sha224_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst);


void sph_sha224_comp(const sph_u32 msg[16], sph_u32 val[8]);


void sph_sha256_init(void *cc);

#ifdef DOXYGEN_IGNORE

void sph_sha256(void *cc, const void *data, size_t len);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_sha256   sph_sha224
#endif


void sph_sha256_close(void *cc, void *dst);


void sph_sha256_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst);

#ifdef DOXYGEN_IGNORE

void sph_sha256_comp(const sph_u32 msg[16], sph_u32 val[8]);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_sha256_comp   sph_sha224_comp
#endif

void sph_sha256_full( void *dst, const void *data, size_t len );
void sha256d(void* hash, const void* data, int len);



void sph_sha256_transform_le( uint32_t *state_out, const uint32_t *data,
                              const uint32_t *state_in );

void sph_sha256_transform_be( uint32_t *state_out, const uint32_t *data,
                              const uint32_t *state_in );


#if SPH_64


#define SPH_SIZE_sha384   384


#define SPH_SIZE_sha512   512


typedef struct {
#ifndef DOXYGEN_IGNORE
	unsigned char buf[128];    
	sph_u64 val[8];
	sph_u64 count;
#endif
} sph_sha384_context;


void sph_sha384_init(void *cc);


void sph_sha384(void *cc, const void *data, size_t len);


void sph_sha384_close(void *cc, void *dst);


void sph_sha384_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst);


void sph_sha384_comp(const sph_u64 msg[16], sph_u64 val[8]);


typedef sph_sha384_context sph_sha512_context;


void sph_sha512_init(void *cc);

#ifdef DOXYGEN_IGNORE

void sph_sha512(void *cc, const void *data, size_t len);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_sha512   sph_sha384
#endif


void sph_sha512_close(void *cc, void *dst);


void sph_sha512_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst);

#ifdef DOXYGEN_IGNORE

void sph_sha512_comp(const sph_u64 msg[16], sph_u64 val[8]);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_sha512_comp   sph_sha384_comp
#endif

#endif

#endif
