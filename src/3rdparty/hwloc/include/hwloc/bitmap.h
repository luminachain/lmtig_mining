



#ifndef HWLOC_BITMAP_H
#define HWLOC_BITMAP_H

#include "hwloc/autogen/config.h"

#include <assert.h>


#ifdef __cplusplus
extern "C" {
#endif






typedef struct hwloc_bitmap_s * hwloc_bitmap_t;

typedef const struct hwloc_bitmap_s * hwloc_const_bitmap_t;





HWLOC_DECLSPEC hwloc_bitmap_t hwloc_bitmap_alloc(void) __hwloc_attribute_malloc;


HWLOC_DECLSPEC hwloc_bitmap_t hwloc_bitmap_alloc_full(void) __hwloc_attribute_malloc;


HWLOC_DECLSPEC void hwloc_bitmap_free(hwloc_bitmap_t bitmap);


HWLOC_DECLSPEC hwloc_bitmap_t hwloc_bitmap_dup(hwloc_const_bitmap_t bitmap) __hwloc_attribute_malloc;


HWLOC_DECLSPEC int hwloc_bitmap_copy(hwloc_bitmap_t dst, hwloc_const_bitmap_t src);





HWLOC_DECLSPEC int hwloc_bitmap_snprintf(char * __hwloc_restrict buf, size_t buflen, hwloc_const_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_asprintf(char ** strp, hwloc_const_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_sscanf(hwloc_bitmap_t bitmap, const char * __hwloc_restrict string);


HWLOC_DECLSPEC int hwloc_bitmap_list_snprintf(char * __hwloc_restrict buf, size_t buflen, hwloc_const_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_list_asprintf(char ** strp, hwloc_const_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_list_sscanf(hwloc_bitmap_t bitmap, const char * __hwloc_restrict string);


HWLOC_DECLSPEC int hwloc_bitmap_taskset_snprintf(char * __hwloc_restrict buf, size_t buflen, hwloc_const_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_taskset_asprintf(char ** strp, hwloc_const_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_taskset_sscanf(hwloc_bitmap_t bitmap, const char * __hwloc_restrict string);





HWLOC_DECLSPEC void hwloc_bitmap_zero(hwloc_bitmap_t bitmap);


HWLOC_DECLSPEC void hwloc_bitmap_fill(hwloc_bitmap_t bitmap);


HWLOC_DECLSPEC int hwloc_bitmap_only(hwloc_bitmap_t bitmap, unsigned id);


HWLOC_DECLSPEC int hwloc_bitmap_allbut(hwloc_bitmap_t bitmap, unsigned id);


HWLOC_DECLSPEC int hwloc_bitmap_from_ulong(hwloc_bitmap_t bitmap, unsigned long mask);


HWLOC_DECLSPEC int hwloc_bitmap_from_ith_ulong(hwloc_bitmap_t bitmap, unsigned i, unsigned long mask);


HWLOC_DECLSPEC int hwloc_bitmap_from_ulongs(hwloc_bitmap_t bitmap, unsigned nr, const unsigned long *masks);





HWLOC_DECLSPEC int hwloc_bitmap_set(hwloc_bitmap_t bitmap, unsigned id);


HWLOC_DECLSPEC int hwloc_bitmap_set_range(hwloc_bitmap_t bitmap, unsigned begin, int end);


HWLOC_DECLSPEC int hwloc_bitmap_set_ith_ulong(hwloc_bitmap_t bitmap, unsigned i, unsigned long mask);


HWLOC_DECLSPEC int hwloc_bitmap_clr(hwloc_bitmap_t bitmap, unsigned id);


HWLOC_DECLSPEC int hwloc_bitmap_clr_range(hwloc_bitmap_t bitmap, unsigned begin, int end);


HWLOC_DECLSPEC int hwloc_bitmap_singlify(hwloc_bitmap_t bitmap);





HWLOC_DECLSPEC unsigned long hwloc_bitmap_to_ulong(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC unsigned long hwloc_bitmap_to_ith_ulong(hwloc_const_bitmap_t bitmap, unsigned i) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_to_ulongs(hwloc_const_bitmap_t bitmap, unsigned nr, unsigned long *masks);


HWLOC_DECLSPEC int hwloc_bitmap_nr_ulongs(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_isset(hwloc_const_bitmap_t bitmap, unsigned id) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_iszero(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_isfull(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_first(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_next(hwloc_const_bitmap_t bitmap, int prev) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_last(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_weight(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_first_unset(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_next_unset(hwloc_const_bitmap_t bitmap, int prev) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_last_unset(hwloc_const_bitmap_t bitmap) __hwloc_attribute_pure;


#define hwloc_bitmap_foreach_begin(id, bitmap) \
do { \
        assert(hwloc_bitmap_weight(bitmap) != -1); \
        for (id = hwloc_bitmap_first(bitmap); \
             (unsigned) id != (unsigned) -1; \
             id = hwloc_bitmap_next(bitmap, id)) {


#define hwloc_bitmap_foreach_end()		\
        } \
} while (0)





HWLOC_DECLSPEC int hwloc_bitmap_or (hwloc_bitmap_t res, hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2);


HWLOC_DECLSPEC int hwloc_bitmap_and (hwloc_bitmap_t res, hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2);


HWLOC_DECLSPEC int hwloc_bitmap_andnot (hwloc_bitmap_t res, hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2);


HWLOC_DECLSPEC int hwloc_bitmap_xor (hwloc_bitmap_t res, hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2);


HWLOC_DECLSPEC int hwloc_bitmap_not (hwloc_bitmap_t res, hwloc_const_bitmap_t bitmap);





HWLOC_DECLSPEC int hwloc_bitmap_intersects (hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_isincluded (hwloc_const_bitmap_t sub_bitmap, hwloc_const_bitmap_t super_bitmap) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_isequal (hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_compare_first(hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2) __hwloc_attribute_pure;


HWLOC_DECLSPEC int hwloc_bitmap_compare(hwloc_const_bitmap_t bitmap1, hwloc_const_bitmap_t bitmap2) __hwloc_attribute_pure;




#ifdef __cplusplus
} 
#endif


#endif 
