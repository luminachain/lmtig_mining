#ifndef _SKEIN_PORT_H_
#define _SKEIN_PORT_H_

#include <limits.h>
#include <stdint.h>

#ifndef RETURN_VALUES
#  define RETURN_VALUES
#  if defined( DLL_EXPORT )
#    if defined( _MSC_VER ) || defined ( __INTEL_COMPILER )
#      define VOID_RETURN    __declspec( dllexport ) void __stdcall
#      define INT_RETURN     __declspec( dllexport ) int  __stdcall
#    elif defined( __GNUC__ )
#      define VOID_RETURN    __declspec( __dllexport__ ) void
#      define INT_RETURN     __declspec( __dllexport__ ) int
#    else
#      error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#    endif
#  elif defined( DLL_IMPORT )
#    if defined( _MSC_VER ) || defined ( __INTEL_COMPILER )
#      define VOID_RETURN    __declspec( dllimport ) void __stdcall
#      define INT_RETURN     __declspec( dllimport ) int  __stdcall
#    elif defined( __GNUC__ )
#      define VOID_RETURN    __declspec( __dllimport__ ) void
#      define INT_RETURN     __declspec( __dllimport__ ) int
#    else
#      error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#    endif
#  elif defined( __WATCOMC__ )
#    define VOID_RETURN  void __cdecl
#    define INT_RETURN   int  __cdecl
#  else
#    define VOID_RETURN  void
#    define INT_RETURN   int
#  endif
#endif



#define ui_type(size)               uint##size##_t
#define dec_unit_type(size,x)       typedef ui_type(size) x
#define dec_bufr_type(size,bsize,x) typedef ui_type(size) x[bsize / (size >> 3)]
#define ptr_cast(x,size)            ((ui_type(size)*)(x))

typedef unsigned int    uint_t;             
typedef uint8_t         u08b_t;             
typedef uint64_t        u64b_t;             

#ifndef RotL_64
#define RotL_64(x,N)    (((x) << (N)) | ((x) >> (64-(N))))
#endif


#ifndef SKEIN_NEED_SWAP 

#define IS_BIG_ENDIAN      4321 
#define IS_LITTLE_ENDIAN   1234 

#if BYTE_ORDER == LITTLE_ENDIAN && !defined(PLATFORM_BYTE_ORDER)
#  define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif

#if BYTE_ORDER == BIG_ENDIAN && !defined(PLATFORM_BYTE_ORDER)
#  define PLATFORM_BYTE_ORDER IS_BIG_ENDIAN
#endif



#if defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#  define PLATFORM_MUST_ALIGN (1)
#ifndef PLATFORM_BYTE_ORDER
#  define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif
#endif

#ifndef   PLATFORM_MUST_ALIGN
#  define PLATFORM_MUST_ALIGN (0)
#endif


#if   PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN
    
#define SKEIN_NEED_SWAP   (1)
#elif PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN
    
#define SKEIN_NEED_SWAP   (0)
#if   PLATFORM_MUST_ALIGN == 0              
#define Skein_Put64_LSB_First(dst08,src64,bCnt) memcpy(dst08,src64,bCnt)
#define Skein_Get64_LSB_First(dst64,src08,wCnt) memcpy(dst64,src08,8*(wCnt))
#endif
#else
#error "Skein needs endianness setting!"
#endif

#endif 


#ifndef Skein_Swap64  
#if     SKEIN_NEED_SWAP
#define Skein_Swap64(w64)                       \
  ( (( ((u64b_t)(w64))       & 0xFF) << 56) |   \
    (((((u64b_t)(w64)) >> 8) & 0xFF) << 48) |   \
    (((((u64b_t)(w64)) >>16) & 0xFF) << 40) |   \
    (((((u64b_t)(w64)) >>24) & 0xFF) << 32) |   \
    (((((u64b_t)(w64)) >>32) & 0xFF) << 24) |   \
    (((((u64b_t)(w64)) >>40) & 0xFF) << 16) |   \
    (((((u64b_t)(w64)) >>48) & 0xFF) <<  8) |   \
    (((((u64b_t)(w64)) >>56) & 0xFF)      ) )
#else
#define Skein_Swap64(w64)  (w64)
#endif
#endif  


#ifndef Skein_Put64_LSB_First
void    Skein_Put64_LSB_First(u08b_t *dst,const u64b_t *src,size_t bCnt)
#ifdef  SKEIN_PORT_CODE 
    { 
    size_t n;

    for (n=0;n<bCnt;n++)
        dst[n] = (u08b_t) (src[n>>3] >> (8*(n&7)));
    }
#else
    ;    
#endif
#endif   


#ifndef Skein_Get64_LSB_First
void    Skein_Get64_LSB_First(u64b_t *dst,const u08b_t *src,size_t wCnt)
#ifdef  SKEIN_PORT_CODE 
    { 
    size_t n;

    for (n=0;n<8*wCnt;n+=8)
        dst[n/8] = (((u64b_t) src[n  ])      ) +
                   (((u64b_t) src[n+1]) <<  8) +
                   (((u64b_t) src[n+2]) << 16) +
                   (((u64b_t) src[n+3]) << 24) +
                   (((u64b_t) src[n+4]) << 32) +
                   (((u64b_t) src[n+5]) << 40) +
                   (((u64b_t) src[n+6]) << 48) +
                   (((u64b_t) src[n+7]) << 56) ;
    }
#else
    ;    
#endif
#endif   

#endif   
