#pragma once
#include "defines.h"
#if USE_SSE
#include "vec3_SSE.h"
#else 
#include "vec3_scalar.h"
#endif

//common functions
inline vec3 fma(vec3 a, vec3 b, vec3 c) {
	return a * b + c;
}
inline vec3 fract(vec3 u) {
	return u - floor(u);
}

inline vec3 vec_eq_tol(vec3 u, vec3 v) {
	vec3 w = fabs(u - v);
	return vec_lt(w, eps);
}
inline float sum(vec3 u)
{
	return dot(1.f, u);
}
inline float avg(vec3 u)
{
	return dot(1.f / 3.f, u);
}

inline vec3 min(vec3 u, vec3 v, vec3 w)
{
	return min(u, min(v, w));
}
inline vec3 max(vec3 u, vec3 v, vec3 w)
{
	return max(u, max(v, w));
}
inline vec3 mix(vec3 x, vec3 y, vec3 t)
{
	return (1.f - t) * x + t * y;
}
inline vec3 clamp(vec3 u, vec3 lo, vec3 hi)
{
	return max(min(u, hi), lo);
}
inline vec3 fixnan(vec3 u)
{
	return max(0.f, u);
}
inline vec3 saturate(vec3 u)
{
	return clamp(u, 0.f, 1.f);
}

inline vec3 reflect(vec3 v, vec3 n)
{
	return v - 2.f * dot(v, n) * n;
}
inline vec3 refract(vec3 v, vec3 n, float eta) {
	float NoV = dot(v, n);
	float k = 1.f - eta * eta * (1.f - NoV * NoV);
	return eta * v - (eta * NoV + sqrtf(fabsf(k))) * n;
}
inline vec3 normal(vec3 u, vec3 v) {
	vec3 uv = cross(u, v);
	vec3 N = norm(uv);
	N.xyz[3] = uv.len();
	return N;
}

inline vec3 fast_sin(vec3 x)
{
	vec3 x2 = x * x;
	return x * (1.f + x2 * (-1.6666656684e-1f + x2 * (8.3330251389e-3f + x2 * (-1.9807418727e-4f + x2 * 2.6019030676e-6f))));
}

inline vec3 sin(vec3 x)
{
	x = min(x, pi - x);
	x = max(x, -pi - x);
	return fast_sin(x);
}
inline vec3 cos(vec3 x)
{
	x = fabs(x - pi) - hpi;
	return fast_sin(x);
}
inline vec3 cossin(float x) {
	float y = fabsf(x - pi) - hpi;
	x = fminf(x, pi - x);
	x = fmaxf(x, -pi - x);
	return fast_sin(vec3(y, x));
}
inline vec3 sincos(float x) {
	float y = fabsf(x - pi) - hpi;
	x = fminf(x, pi - x);
	x = fmaxf(x, -pi - x);
	return fast_sin(vec3(x, y));
}

inline float luminance(vec3 rgb)
{
	return dot(vec3(0.2126f, 0.7152f, 0.0722f), rgb);
}

inline void rgb(vec3 col, uint& rgb)
{
	col /= col.w();
#if GAMMA2
	col = sqrt(col);
#endif
	col = saturate(col);
	col *= 255.f;
	col += 0.5f * ravec();
	rgb = pack_rgb(col.x(), col.y(), col.z());
}
inline void bgr(vec3 col, uint& bgr)
{
	//divide by pixels sample count
	col /= col.w();
#if GAMMA2
	//gamma correction to srgb
	col = sqrt(col);
#endif
	//clamp 0 - 1
	col = saturate(col);
	//multiply by 8bit max value
	col *= 255.f;
	//deband
	col += 0.5f * ravec();
	//store to bgr
	bgr = pack_bgr(col.x(), col.y(), col.z());
}

inline vec3 unrgb(const uint& rgb) {
	vec3 v;
	v.xyz[0] = rgb & 0x000000ff;
	v.xyz[1] = (rgb >> 8) & 0x000000ff;
	v.xyz[2] = (rgb >> 16) & 0x000000ff;
	v.xyz[3] = (rgb >> 24) & 0x000000ff;
	return v * (1.f / 255.f);
}

inline vec3 med9(vec3* x) {
	auto cmp_lum = [](const vec3& a, const vec3& b) {
		return luminance(a) < luminance(b);
	};
	std::nth_element(x, x + 4, x + 9, cmp_lum);
	return x[4];
}
inline vec3 avg9(vec3* x) {
	vec3 sum;
	for (int i = 0; i < 9; i++)
		sum += x[i];
	return sum * (1.f / 9.f);
}
inline vec3 med(vec3* x, const int n) {
	auto cmp_lum = [](const vec3& a, const vec3& b) {
		return luminance(a) < luminance(b);
	};
	std::nth_element(x, x + n / 2, x + n, cmp_lum);
	return x[n / 2];
}
using vec4 = vec3;