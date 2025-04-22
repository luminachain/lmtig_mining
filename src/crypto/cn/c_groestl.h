#ifndef __hash_h
#define __hash_h

#include <stdint.h>

#include "hash.h"


#define ROWS 8
#define LENGTHFIELDLEN ROWS
#define COLS512 8

#define SIZE512 (ROWS*COLS512)

#define ROUNDS512 10
#define HASH_BIT_LEN 256

#define ROTL32(v, n) ((((v)<<(n))|((v)>>(32-(n))))&li_32(ffffffff))


#define li_32(h) 0x##h##u
#define EXT_BYTE(var,n) ((uint8_t)((uint32_t)(var) >> (8*n)))
#define u32BIG(a)				\
  ((ROTL32(a,8) & li_32(00FF00FF)) |		\
   (ROTL32(a,24) & li_32(FF00FF00)))



typedef struct {
  uint32_t chaining[SIZE512/sizeof(uint32_t)];            
  uint32_t block_counter1,
  block_counter2;         
  BitSequence buffer[SIZE512];      
  int buf_ptr;              
  int bits_in_last_byte;    
} groestlHashState;


void groestl(const BitSequence*, DataLength, BitSequence*);




#endif 
