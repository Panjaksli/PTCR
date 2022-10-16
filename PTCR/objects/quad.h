#pragma once
#include "obj.h"
//quad vec3(Q) vec3(U) vec3(V) vec3(N), 4*4*4 = 64B
class quad {
public:
	quad() {}
	quad(vec3 _A, vec3 _B, vec3 _C) :Q(_A), U(_B - _A), V(_C - _A), N(normal(U, V)) {}
	quad(vec3 _Q, vec3 _U, vec3 _V, bool param) :Q(_Q), U(_U), V(_V), N(normal(U, V)) {}

	inline aabb get_box()const {
		return aabb(Q, Q + U, Q + V) + aabb(Q + U + V, Q + U, Q + V);
	}

	inline quad trans(const matrix& T) const {
		return quad(Q + T.P(), T * U, T * V, 1);
	}
	inline bool hit(const ray& r, hitrec& rec) const
	{
		vec3 pV = cross(r.D, V);
		float D = dot(U, pV);
		float iD = 1.f / D;
		vec3 tV = r.O - Q;
		vec3 qV = cross(tV, U);
		float u = dot(tV, pV) * iD;
		float v = dot(r.D, qV) * iD;
		float t = dot(V, qV) * iD;
		if (inside(t, eps2, rec.t) && within(u, 0.f, 1.f) && within(v, 0.f, 1.f))
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
	inline float pdf(const ray& r)const {
		hitrec rec;
		if (!hit(r, rec))return 0;
		float S = N.w;
		float NoL = absdot(N, r.D);
		return rec.t * rec.t / (S * NoL);
	}
	inline vec3 rand_to(vec3 O) const {
		float r[2]; rafl_tuple(r);
		vec3 P = Q + r[0] * U + r[1] * V;
		vec3 L = norm(P - O);
		return L;
	}
	inline vec3 rand_from() const {
		float r[2]; rafl_tuple(r);
		vec3 P = Q + r[0] * U + r[1] * V;
		vec3 L = onb(N).local(sa_cos());
		return L;
	}
	vec3 Q, U, V, N;
};