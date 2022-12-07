#pragma once
#include "obj.h"
class poly {
public:
	poly() {}
	poly(vec3 _A, vec3 _B, vec3 _C) :Q(_A), U(_B - _A), V(_C - _A), N(poly_nis(U, V)) {}
	poly(vec3 _Q, vec3 _U, vec3 _V, bool param) :Q(_Q), U(_U), V(_V), N(poly_nis(U, V)) {}

	inline bool hit(const ray& r, hitrec& rec) const
	{
		//Moller-Trumbore algorithm, based on:
		//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
		vec3 pV = cross(r.D, V);
		float D = dot(U, pV);
		float iD = 1.f / D;
		vec3 tV = r.O - Q;
		vec3 qV = cross(tV, U);
		float u = dot(tV, pV) * iD;
		float v = dot(r.D, qV) * iD;
		float t = dot(V, qV) * iD;
		if (within(u, 0.f, 1.f) && within(v, 0.f, 1.f - u) && inside(t, eps2, rec.t))
		{
			bool face = D > 0;
			rec.N = face ? N : -N;
			rec.P = Q + u * U + v * V;
			rec.t = t;
			rec.u = u;
			rec.v = v;
			rec.face = face;
			return true;
		}
		return false;
	}
	inline aabb get_box()const {
		return aabb(Q, Q + U, Q + V).padded();
	}
	inline poly trans(const matrix& T) const {
		vec3 q = T * Q + T.P();
		vec3 u = T * U;
		vec3 v = T * V;
		return poly(q, u, v, true);
	}
	inline float pdf(const ray& r)const {
		hitrec rec;
		if (!hit(r, rec))return 0;
		float iS = N.w();
		float NoL = absdot(N, r.D);
		return rec.t * rec.t * iS / NoL;
	}
	inline vec3 rand_to(vec3 O) const {
		float r[2]; rafl_tuple(r);
		if (r[0] + r[1] > 1.f) {
			r[0] = 1.f - r[0];
			r[1] = 1.f - r[1];
		}
		vec3 P = Q + r[0] * U + r[1] * V;
		vec3 L = norm(P - O);
		return L;
	}
	inline vec3 rand_from() const {
		float r[2]; rafl_tuple(r);
		if (r[0] + r[1] > 1.f) {
			r[0] = 1.f - r[0];
			r[1] = 1.f - r[1];
		}
		//vec3 P = Q + r[0] * U + r[1] * V;
		vec3 L = onb(N).world(sa_cos());
		return L;
	}
	vec3 Q, U, V, N;
};



