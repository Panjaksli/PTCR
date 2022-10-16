#pragma once
#include <immintrin.h>

inline __m128i _mm_xorshift32_epi32() {
	thread_local static __m128i x{ 0x293a0c203c4c9ac3, 0x38304da1a654cb78 };
	x = _mm_xor_si128(x, _mm_slli_epi32(x, 13));
	x = _mm_xor_si128(x, _mm_srli_epi32(x, 17));
	x = _mm_xor_si128(x, _mm_slli_epi32(x, 5));
	return x;
}
inline __m128i _mm_fastrand_epi32() {
	thread_local static __m128i x{ 0x293a0c203c4c9ac3, 0x38304da1a654cb78 };
	x = (214013U * x + 2531011U);
	return x;
}
inline __m128 _mm_rafl_ps() {
	__m128i x = _mm_srli_epi32(_mm_xorshift32_epi32(),9);
	x = _mm_and_si128(_mm_set1_epi32(0x007FFFFF), x);
	x = _mm_or_si128(_mm_set1_epi32(0x3f800000), x);
	return _mm_castsi128_ps(x) - 1.f;
}
inline __m128 _mm_rafl_ps(float max) {
	return max * _mm_rafl_ps();
}
inline __m128 _mm_rafl_ps(float min, float max) {
	return min + (max - min) * _mm_rafl_ps();
}

inline void rafl_quad(float r12[4]) {
	_mm_storeu_ps(r12, _mm_rafl_ps());
}

inline __m128 _mm_abs_ps(__m128 x) {
	const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	return _mm_andnot_ps(mask, x);
};

inline __m128 _mm_sign_ps(__m128 x, __m128 y) {
	const __m128 mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	__m128 sign = _mm_and_ps(mask, y);
	__m128 absval = _mm_andnot_ps(mask, x);
	return _mm_or_ps(sign, absval);
};
inline __m128 _mm_sign_ps(__m128 x) {
	return _mm_sign_ps(_mm_set1_ps(1.f), x);
}

inline __m128 _mm_fsqrt_ps(__m128 n) {
	__m128i i = _mm_castps_si128(n);
	i = _mm_srli_epi32(i, 1);
	i = _mm_add_epi32(i, _mm_set1_epi32(0x1fbd5f5f));
	return _mm_castsi128_ps(i);
}

inline __m128 sqrt(__m128 n) {
	__asm__("sqrtps %1, %0" : "+x" (n));
	return n;
}
inline __m128 div(__m128 a, __m128 b) {
	__asm__("divps %0, %1, %0" : "+x" (b) : "x" (a));
	return b;
}

/*
https://geometrian.com/programming/tutorials/cross-product/index.php
*/
inline __m128 _mm_cross_ps(__m128 const& u, __m128 const& v) {
	__m128 tmp0 = _mm_shuffle_ps(u, u, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 tmp1 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 tmp2 = _mm_shuffle_ps(u, u, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 tmp3 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 2, 1));
	return _mm_sub_ps(
		_mm_mul_ps(tmp0, tmp1),
		_mm_mul_ps(tmp2, tmp3)
	);
}
