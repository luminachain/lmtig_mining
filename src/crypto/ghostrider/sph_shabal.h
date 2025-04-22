


#ifndef SPH_SHABAL_H__
#define SPH_SHABAL_H__

#include "sph_types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#define SPH_SIZE_shabal192 192


#define SPH_SIZE_shabal224 224


#define SPH_SIZE_shabal256 256


#define SPH_SIZE_shabal384 384


#define SPH_SIZE_shabal512 512


typedef struct {
#ifndef DOXYGEN_IGNORE
  unsigned char buf[64]; 
  size_t ptr;
  sph_u32 A[12], B[16], C[16];
  sph_u32 Whigh, Wlow;
#endif
} sph_shabal_context;


typedef sph_shabal_context sph_shabal192_context;


typedef sph_shabal_context sph_shabal224_context;


typedef sph_shabal_context sph_shabal256_context;


typedef sph_shabal_context sph_shabal384_context;


typedef sph_shabal_context sph_shabal512_context;


void sph_shabal192_init(void *cc);


void sph_shabal192(void *cc, const void *data, size_t len);


void sph_shabal192_close(void *cc, void *dst);


void sph_shabal192_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                     void *dst);


void sph_shabal224_init(void *cc);


void sph_shabal224(void *cc, const void *data, size_t len);


void sph_shabal224_close(void *cc, void *dst);


void sph_shabal224_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                     void *dst);


void sph_shabal256_init(void *cc);


void sph_shabal256(void *cc, const void *data, size_t len);


void sph_shabal256_close(void *cc, void *dst);


void sph_shabal256_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                     void *dst);


void sph_shabal384_init(void *cc);


void sph_shabal384(void *cc, const void *data, size_t len);


void sph_shabal384_close(void *cc, void *dst);


void sph_shabal384_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                     void *dst);


void sph_shabal512_init(void *cc);


void sph_shabal512(void *cc, const void *data, size_t len);


void sph_shabal512_close(void *cc, void *dst);


void sph_shabal512_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                     void *dst);

#ifdef __cplusplus
}
#endif

#endif
