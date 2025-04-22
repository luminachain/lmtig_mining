


#ifndef SPH_WHIRLPOOL_H__
#define SPH_WHIRLPOOL_H__

#include "sph_types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if SPH_64


#define SPH_SIZE_whirlpool 512


#define SPH_SIZE_whirlpool0 512


#define SPH_SIZE_whirlpool1 512


typedef struct {
#ifndef DOXYGEN_IGNORE
  unsigned char buf[64]; 
  sph_u64 state[8];
#if SPH_64
  sph_u64 count;
#else
  sph_u32 count_high, count_low;
#endif
#endif
} sph_whirlpool_context;


void sph_whirlpool_init(void *cc);


void sph_whirlpool(void *cc, const void *data, size_t len);


void sph_whirlpool_close(void *cc, void *dst);

#define sph_whirlpool512_full(cc, dst, data, len)                              \
  do {                                                                         \
    sph_whirlpool_init(cc);                                                    \
    sph_whirlpool(cc, data, len);                                              \
    sph_whirlpool_close(cc, dst);                                              \
  } while (0)


typedef sph_whirlpool_context sph_whirlpool0_context;

#ifdef DOXYGEN_IGNORE

void sph_whirlpool0_init(void *cc);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_whirlpool0_init sph_whirlpool_init
#endif


void sph_whirlpool0(void *cc, const void *data, size_t len);


void sph_whirlpool0_close(void *cc, void *dst);


typedef sph_whirlpool_context sph_whirlpool1_context;

#ifdef DOXYGEN_IGNORE

void sph_whirlpool1_init(void *cc);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_whirlpool1_init sph_whirlpool_init
#endif


void sph_whirlpool1(void *cc, const void *data, size_t len);


void sph_whirlpool1_close(void *cc, void *dst);

#endif

#ifdef __cplusplus
}
#endif

#endif
