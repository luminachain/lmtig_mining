

#include <assert.h>
#include "crypto/randomx/reciprocal.h"


uint64_t randomx_reciprocal(uint64_t divisor) {

	assert(divisor != 0);

	const uint64_t p2exp63 = 1ULL << 63;

	uint64_t quotient = p2exp63 / divisor, remainder = p2exp63 % divisor;

	unsigned bsr = 0; 

	for (uint64_t bit = divisor; bit > 0; bit >>= 1)
		bsr++;

	for (unsigned shift = 0; shift < bsr; shift++) {
		if (remainder >= divisor - remainder) {
			quotient = quotient * 2 + 1;
			remainder = remainder * 2 - divisor;
		}
		else {
			quotient = quotient * 2;
			remainder = remainder * 2;
		}
	}

	return quotient;
}

#if !RANDOMX_HAVE_FAST_RECIPROCAL

uint64_t randomx_reciprocal_fast(uint64_t divisor) {
	return randomx_reciprocal(divisor);
}

#endif
