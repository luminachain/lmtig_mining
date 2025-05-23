

#include <cfenv>
#include <cmath>
#include "crypto/randomx/common.hpp"
#include "crypto/randomx/intrin_portable.h"
#include "crypto/randomx/blake2/endian.h"

#if defined(__SIZEOF_INT128__)
	typedef unsigned __int128 uint128_t;
	typedef __int128 int128_t;
	uint64_t mulh(uint64_t a, uint64_t b) {
		return ((uint128_t)a * b) >> 64;
	}
	int64_t smulh(int64_t a, int64_t b) {
		return ((int128_t)a * b) >> 64;
	}
	#define HAVE_MULH
	#define HAVE_SMULH
#endif

#if defined(_MSC_VER)
	#define HAS_VALUE(X) X ## 0
	#define EVAL_DEFINE(X) HAS_VALUE(X)
	#include <intrin.h>
	#include <stdlib.h>

	uint64_t rotl64(uint64_t x, unsigned int c) {
		return _rotl64(x, c);
	}
	uint64_t rotr64(uint64_t x, unsigned int c) {
		return _rotr64(x, c);
	}
	#define HAVE_ROTL64
	#define HAVE_ROTR64

	#if EVAL_DEFINE(__MACHINEARM64_X64(1))
		uint64_t mulh(uint64_t a, uint64_t b) {
			return __umulh(a, b);
		}
		#define HAVE_MULH
	#endif

	#if EVAL_DEFINE(__MACHINEX64(1))
		int64_t smulh(int64_t a, int64_t b) {
			int64_t hi;
			_mul128(a, b, &hi);
			return hi;
		}
		#define HAVE_SMULH
	#endif

	static void setRoundMode_(uint32_t mode) {
		_controlfp(mode, _MCW_RC);
	}
	#define HAVE_SETROUNDMODE_IMPL
#endif

#ifndef HAVE_ROTR64
	uint64_t rotr64(uint64_t a, unsigned int b) {
		return (a >> b) | (a << (-b & 63));
	}
	#define HAVE_ROTR64
#endif

#ifndef HAVE_ROTL64
	uint64_t rotl64(uint64_t a, unsigned int b) {
		return (a << b) | (a >> (-b & 63));
	}
	#define HAVE_ROTL64
#endif

#ifndef HAVE_MULH
	#define LO(x) ((x)&0xffffffff)
	#define HI(x) ((x)>>32)
	uint64_t mulh(uint64_t a, uint64_t b) {
		uint64_t ah = HI(a), al = LO(a);
		uint64_t bh = HI(b), bl = LO(b);
		uint64_t x00 = al * bl;
		uint64_t x01 = al * bh;
		uint64_t x10 = ah * bl;
		uint64_t x11 = ah * bh;
		uint64_t m1 = LO(x10) + LO(x01) + HI(x00);
		uint64_t m2 = HI(x10) + HI(x01) + LO(x11) + HI(m1);
		uint64_t m3 = HI(x11) + HI(m2);

		return (m3 << 32) + LO(m2);
	}
	#define HAVE_MULH
#endif

#ifndef HAVE_SMULH
	int64_t smulh(int64_t a, int64_t b) {
		int64_t hi = mulh(a, b);
		if (a < 0LL) hi -= b;
		if (b < 0LL) hi -= a;
		return hi;
	}
	#define HAVE_SMULH
#endif

#ifdef RANDOMX_DEFAULT_FENV

#	ifndef HAVE_SETROUNDMODE_IMPL
	static void setRoundMode_(uint32_t mode) {
		fesetround(mode);
	}
#	endif

void rx_reset_float_state() {
	setRoundMode_(FE_TONEAREST);
	rx_set_double_precision(); 
}

void rx_set_rounding_mode(uint32_t mode) {
	switch (mode & 3) {
	case RoundDown:
		setRoundMode_(FE_DOWNWARD);
		break;
	case RoundUp:
		setRoundMode_(FE_UPWARD);
		break;
	case RoundToZero:
		setRoundMode_(FE_TOWARDZERO);
		break;
	case RoundToNearest:
		setRoundMode_(FE_TONEAREST);
		break;
	default:
		UNREACHABLE;
	}
}

#endif

#ifdef RANDOMX_USE_X87

#ifdef _M_IX86

void rx_set_double_precision() {
	_control87(_PC_53, _MCW_PC);
}

#elif defined(__i386)

void rx_set_double_precision() {
	uint16_t volatile x87cw;
	asm volatile("fstcw %0" : "=m" (x87cw));
	x87cw &= ~0x300;
	x87cw |= 0x200;
	asm volatile("fldcw %0" : : "m" (x87cw));
}

#endif

#endif 

union double_ser_t {
	double f;
	uint64_t i;
};

double loadDoublePortable(const void* addr) {
	double_ser_t ds;
	ds.i = load64(addr);
	return ds.f;
}
