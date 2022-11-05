#pragma once
#include "vec3.h"
#include "obj_list.h"

class cos_pdf {
public:
	cos_pdf() {}
	cos_pdf(vec3 _N, vec3 _L) : N(_N), L(_L) {}

	inline float value(vec3 V) const {
		return dot(N, V) * ipi;
	}
	inline vec3 generate() const {
		return L;
	}
	vec3 N, L;
};
class bid_pdf
{

};
class lig_pdf {
public:
	lig_pdf(const obj_list& _obj, vec3 _O) : obj(_obj), O(_O) {}

	inline float value(vec3 V) const {
		return obj.pdf(ray(O, V));
	}
	inline vec3 generate() const {
		return obj.rand_to(O);
	}

	const obj_list& obj;
	vec3 O;
};
constexpr float sun_h = 3e10f;
constexpr float sun_r = 2.3e9f;
constexpr float sun_angle = sun_h / sun_r;
const float maxdp = sun_angle / sqrtf(2.f + sun_angle * sun_angle);
class sun_pdf {
public:
	sun_pdf(const matrix& _T, vec3 _O) : T(_T), O(_O) {}
	inline float value(vec3 V) const {
		float dp = dot(V, T * vec3(0, 1, 0));
		constexpr float pdf = sun_angle * sun_angle * ipi;
		if(dp<maxdp)return 0;
		return pdf / dp;
	}
	inline vec3 generate() const {
		float r[2]; rafl_tuple(r);
		vec3 V = T * norm(vec3(r[0], sun_angle, r[1]));
		return V;
	}
	const matrix&T;
	vec3 O;
};
template <class P1, class P2>
class mix_pdf {
public:
	mix_pdf(const P1& _p1, const P2& _p2) :p1(_p1), p2(_p2) {}
	inline float value(vec3 V) const {
		return 0.5f * (p1.value(V) + p2.value(V));
	}
	inline vec3 generate() const {
		if (rafl() < 0.5f)
			return p1.generate();
		else
			return p2.generate();
	}

	const P1& p1;
	const P2& p2;
};
template <class P1, class P2>
class bias_pdf {
public:
	bias_pdf(const P1& _p1, const P2& _p2, float _b = 0.5f) :p1(_p1), p2(_p2),b(_b) {}
	inline float value(vec3 V) const {
		return b * p1.value(V) + (1.f - b) * p2.value(V);
	}
	inline vec3 generate() const {
		if (rafl() < b)
			return p1.generate();
		else
			return p2.generate();
	}

	const P1& p1;
	const P2& p2;
	float b;
};
template <class P1, class P2, class P3>
class mix3_pdf {
public:
	mix3_pdf(const P1& _p1, const P2& _p2, const P3& _p3) :p1(_p1), p2(_p2),p3(_p3) {}
	inline float value(vec3 V) const {
		return (1.f / 3.f) * (p1.value(V) + p2.value(V) + p3.value(V));
	}
	inline vec3 generate() const {
		if (rafl() < (1.f / 3.f))
			return p1.generate();
		else if (rafl() < (2.f / 3.f))
			return p2.generate();
		else
			return p3.generate();
	}

	const P1& p1;
	const P2& p2;
	const P3& p3;
};

