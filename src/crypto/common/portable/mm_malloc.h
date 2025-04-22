

#ifndef XMRIG_MM_MALLOC_PORTABLE_H
#define XMRIG_MM_MALLOC_PORTABLE_H


#if defined(XMRIG_ARM) && !defined(__clang__)
#include <stdlib.h>


#ifndef __cplusplus
extern
#else
extern "C"
#endif
int posix_memalign(void **__memptr, size_t __alignment, size_t __size);


static __inline__ void *__attribute__((__always_inline__, __malloc__)) _mm_malloc(size_t __size, size_t __align)
{
    if (__align == 1) {
        return malloc(__size);
    }

    if (!(__align & (__align - 1)) && __align < sizeof(void *)) {
        __align = sizeof(void *);
    }

    void *__mallocedMemory;
    if (posix_memalign(&__mallocedMemory, __align, __size)) {
        return nullptr;
    }

    return __mallocedMemory;
}


static __inline__ void __attribute__((__always_inline__)) _mm_free(void *__p)
{
    free(__p);
}
#elif defined(_WIN32) && !defined(__GNUC__)
#   include <malloc.h>
#else
#   include <mm_malloc.h>
#endif


#endif 
