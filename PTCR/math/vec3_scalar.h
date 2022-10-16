#pragma once
#include "util.h"
class __declspec(align(16)) vec3
{
public:
	vec3() : _xyz{} {}
	vec3(float t) : _xyz{ t,t,t,t } {}
	vec3(vec3 v, float w) :_xyz{ v.x,v.y,v.z,w } {}
	vec3(float x, float y, float z = 0, float w = 0) : _xyz{ x,y,z,w } {}

	inline float operator[](uint i) const { return _xyz[i]; }
	inline float& operator[](uint i) { return _xyz[i]; }
	inline vec3 operator-() const { return vec3(-x, -y, -z, -w); }
	inline vec3& operator+=(vec3 u)
	{
		x += u.x;
		y += u.y;
		z += u.z;
		w += u.w;
		return *this;
	}
	inline vec3& operator-=(vec3 u)
	{
		x -= u.x;
		y -= u.y;
		z -= u.z;
		w -= u.w;
		return *this;
	}
	inline vec3& operator*=(vec3 u)
	{
		x *= u.x;
		y *= u.y;
		z *= u.z;
		w *= u.w;
		return *this;
	}
	inline vec3& operator/=(vec3 u)
	{
		x /= u.x;
		y /= u.y;
		z /= u.z;
		w /= u.w;
		return *this;
	}
	inline float len2() const {
		return x * x + y * y + z * z;
	}
	inline float len() const { return sqrtf(len2()); }
	inline vec3 dir() const
	{
		float inv = 1.f / len();
		return vec3(x * inv, y * inv, z * inv, 0);
	}
	void print()const {
		printf("%.8f %.8f %.8f\n", x, y, z);
	}
	void print4()const {
		printf("%f %f %f %f\n", x, y, z, w);
	}
	void printM()const {
		printf("%f %f %f %f\n", x, y, z, len());
	}
	union {
		struct {
			float x, y, z, w;
		};
		float _xyz[4];
	};

};

inline vec3 norm(vec3 u) { return u.dir(); }
inline vec3 operator+(vec3 u, vec3 v) { return u += v; }
inline vec3 operator-(vec3 u, vec3 v) { return u -= v; }
inline vec3 operator*(vec3 u, vec3 v) { return u *= v; }
inline vec3 operator/(vec3 u, vec3 v) { return u /= v; }
inline vec3 fma(vec3 a, vec3 b, vec3 c) {
	return a * b + c;
}
inline float operator&(vec3 u, vec3 v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}
inline vec3 operator%(vec3 u, vec3 v)
{
	return vec3(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}
inline vec3 sqrt(vec3 u)
{
	return vec3(sqrtf(u.x), sqrtf(u.y), sqrtf(u.z), sqrtf(u.w));
}
inline vec3 abs(vec3 u)
{
	return vec3(fabsf(u.x), fabsf(u.y), fabsf(u.z), fabsf(u.w));
}
inline vec3 fabs(vec3 u)
{
	return abs(u);
}
inline vec3 sign(vec3 u) {
	return vec3(signf(u.x), signf(u.y), signf(u.z), signf(u.w));
}
inline vec3 toint(vec3 u) {
	return vec3(int(u.x), int(u.y), int(u.z), int(u.w));
}
inline vec3 floor(vec3 u) {
	return vec3(floorf(u.x), floorf(u.y), floorf(u.z), floorf(u.w));
}
inline vec3 ceil(vec3 u) {
	return vec3(ceilf(u.x), ceilf(u.y), ceilf(u.z), ceilf(u.w));
}
inline vec3 fract(vec3 u) {
	return u - floor(u);
}
inline vec3 cross(vec3 u, vec3 v) { return u % v; }
inline float dot(vec3 u, vec3 v) { return u & v; }
inline float dot4(vec3 u, vec3 v) { return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w; }
inline float posdot(vec3 u, vec3 v) { return fmaxf(0.f, u & v); }
inline float absdot(vec3 u, vec3 v) { return fabsf(u & v); }
inline float dotabs(vec3 u, vec3 v) { return (abs(u) & abs(v)); }
inline vec3 vec_gt(vec3 u, vec3 v) {
	return vec3(u.x > v.x, u.y > v.y, u.z > v.z, u.w > v.w);
}
inline vec3 vec_lt(vec3 u, vec3 v) {
	return vec3(u.x < v.x, u.y < v.y, u.z < v.z, u.w < v.w);
}
inline vec3 vec_eq(vec3 u, vec3 v) {
	return vec3(u.x == v.x, u.y == v.y, u.z == v.z, u.w == v.w);
}
inline vec3 vec_neq(vec3 u, vec3 v) {
	return vec3(u.x != v.x, u.y != v.y, u.z != v.z, u.w != v.w);
}
inline vec3 vec_eq_tol(vec3 u, vec3 v) {
	vec3 w = fabs(u - v);
	return vec_lt(w, eps);
}
inline bool eq(vec3 u, vec3 v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}
inline bool neq(vec3 u, vec3 v)
{
	return !eq(u, v);
}
inline bool eq_tol(vec3 u, vec3 v, float tol = eps)
{
	vec3 w = abs(u - v);
	return w.x < eps&& w.y < eps&& w.z < eps;
}
inline bool eq0(vec3 u)
{
	return u.x == 0 && u.y == 0 && u.z == 0;
}
inline bool neq0(vec3 u)
{
	return !eq0(u);
}

inline bool near0(vec3 u)
{
	u = fabs(u);
	return u.x < eps&& u.y < eps&& u.z < eps;
}
inline bool not0(vec3 u) {
	return !near0(u);
}

inline float min(vec3 u)
{
	return fminf(u.x, fminf(u.y, u.z));
}
inline float max(vec3 u)
{
	return fmaxf(u.x, fmaxf(u.y, u.z));
}
inline vec3 min(vec3 u, vec3 v)
{
	return vec3(fminf(u.x, v.x), fminf(u.y, v.y), fminf(u.z, v.z), fminf(u.w, v.w));
}
inline vec3 max(vec3 u, vec3 v)
{
	return vec3(fmaxf(u.x, v.x), fmaxf(u.y, v.y), fmaxf(u.z, v.z), fmaxf(u.w, v.w));
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

inline vec3 reflect(const vec3 v, const vec3 n)
{
	return v - 2.f * dot(v, n) * n;
}
inline vec3 normal(const vec3 u, const vec3 v) {
	vec3 uv = cross(u, v);
	vec3 N = norm(uv);
	N.w = uv.len();
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

inline vec3 ravec() {
	return vec3(rafl(), rafl(), rafl());
}
inline vec3 ravec(float min, float max) {
	return vec3(rafl(min, max), rafl(min, max), rafl(min, max));
}
inline vec3 ra_disk()
{
	vec3 u(0);
	rafl_tuple(u._xyz);
	while (u.len2() >= 1.f)
		rafl_tuple(u._xyz);
	return u;
}
inline vec3 ra_sph()
{
	vec3 u(ravec(-1, 1));
	while (u.len2() >= 1.f)
		u = ravec(-1, 1);
	return u;
}
inline vec3 ra_hem(vec3 v)
{
	vec3 u = ra_sph();
	if (dot(u, v) > 0)
		return u;
	else
		return -u;
}

inline float luminance(vec3 rgb)
{
	return (0.2126f * rgb.x + 0.7152f * rgb.y + 0.0722f * rgb.z);
}
inline float sum3(vec3 rgb)
{
	return (rgb.x + rgb.y + rgb.z) * (1.f / 3.f);
}
inline vec3 unrgb(const uint& rgb) {
	vec3 v;
	v.x = rgb & 0x000000ff;
	v.y = (rgb >> 8) & 0x000000ff;
	v.z = (rgb >> 16) & 0x000000ff;
	v.w = (rgb >> 24) & 0x000000ff;
	return v * (1.f / 255.f);
}
inline void rgb(vec3 col, uint& rgb)
{
	col /= col.w;
#if GAMMA2
	col = sqrt(col);
#endif
	col = saturate(col);
	col *= 255.f;
	rgb = pack_rgb(col.x, col.y, col.z);
}

inline void rgb_avg(vec3 col, uint& rgb)
{
	col /= col.w;
#if GAMMA2
	col = sqrt(col);
#endif
	col = saturate(col);
	vec3 pre = unrgb(rgb);
	col = 0.5f * (col + pre);
	col *= 255.f;

	rgb = pack_rgb(col.x, col.y, col.z);
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