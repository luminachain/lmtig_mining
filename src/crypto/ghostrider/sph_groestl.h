


#ifndef SPH_GROESTL_H__
#define SPH_GROESTL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sph_types.h"
#include <stddef.h>


#define SPH_SIZE_groestl224 224


#define SPH_SIZE_groestl256 256


#define SPH_SIZE_groestl384 384


#define SPH_SIZE_groestl512 512


typedef struct {
#ifndef DOXYGEN_IGNORE
  unsigned char buf[64]; 
  size_t ptr;
  union {
#if SPH_64
    sph_u64 wide[8];
#endif
    sph_u32 narrow[16];
  } state;
#if SPH_64
  sph_u64 count;
#else
  sph_u32 count_high, count_low;
#endif
#endif
} sph_groestl_small_context;


typedef sph_groestl_small_context sph_groestl224_context;


typedef sph_groestl_small_context sph_groestl256_context;


typedef struct {
#ifndef DOXYGEN_IGNORE
  unsigned char buf[128]; 
  size_t ptr;
  union {
#if SPH_64
    sph_u64 wide[16];
#endif
    sph_u32 narrow[32];
  } state;
#if SPH_64
  sph_u64 count;
#else
  sph_u32 count_high, count_low;
#endif
#endif
} sph_groestl_big_context;


typedef sph_groestl_big_context sph_groestl384_context;


typedef sph_groestl_big_context sph_groestl512_context;


void sph_groestl224_init(void *cc);


void sph_groestl224(void *cc, const void *data, size_t len);


void sph_groestl224_close(void *cc, void *dst);


void sph_groestl224_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                      void *dst);


void sph_groestl256_init(void *cc);


void sph_groestl256(void *cc, const void *data, size_t len);


void sph_groestl256_close(void *cc, void *dst);


void sph_groestl256_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                      void *dst);


void sph_groestl384_init(void *cc);


void sph_groestl384(void *cc, const void *data, size_t len);


void sph_groestl384_close(void *cc, void *dst);


void sph_groestl384_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                      void *dst);


void sph_groestl512_init(void *cc);


void sph_groestl512(void *cc, const void *data, size_t len);


void sph_groestl512_close(void *cc, void *dst);


void sph_groestl512_addbits_and_close(void *cc, unsigned ub, unsigned n,
                                      void *dst);

#ifdef __cplusplus
}
#endif

#endif
