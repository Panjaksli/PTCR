#pragma once
#include "vec3.h"
#if DEBUG
static bool use_normal_maps = 1;
#else
constexpr bool use_normal_maps = 1;
#endif
class onb
{
public:
	onb() {}
	onb(const vec3& w) { build(w); }
	inline vec3 operator[](int i) const { return uvw[i]; }
	inline vec3& operator[](int i) { return uvw[i]; }

	inline vec3 u() const { return uvw[0]; }
	inline vec3 v() const { return uvw[1]; }
	inline vec3 w() const { return uvw[2]; }

	inline vec3 world(const vec3& a) const {
		return a.x * u() + a.y * v() + a.z * w();
	}
	inline vec3 local(const vec3& a)const {
		return vec3(dot(a, u()), dot(a, v()), dot(a, w()));
	}
	/*
	branchlessONB
	https://graphics.pixar.com/library/OrthonormalB/paper.pdf
	*/
	inline void build(const vec3& w) {
		vec3 u, v;
		float sign = signf(w.z);
		const float a = -1.0f / (sign + w.z);
		const float b = w.x * w.y * a;
		u = vec3(1.0f + sign * w.x * w.x * a, sign * b, -sign * w.x);
		v = vec3(b, sign + w.y * w.y * a, -w.y);
		uvw[0] = u;
		uvw[1] = v;
		uvw[2] = w;
	}

public:
	vec3 uvw[3];
};
//map is texture value (0 to 1)
inline vec3 normal_map(vec3 N, vec3 map)
{
	if (!use_normal_maps||eq(map, vec3(0.5f, 0.5f, 1.f)))return N;
	onb uvw(N);
	map = 2.f * map - 1.f;
	return norm(uvw.world(map));
}
