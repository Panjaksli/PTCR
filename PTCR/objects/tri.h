#pragma once
#include "obj.h"
class tri {
public:
	tri() {}
	tri(vec3 _A, vec3 _B, vec3 _C) :Q(_A), U(_B - _A), V(_C - _A), N(normal(U, V)) {}
	tri(vec3 _Q, vec3 _U, vec3 _V, bool param) :Q(_Q), U(_U), V(_V), N(normal(U, V)) {}

	inline aabb get_box()const {
		return (aabb(Q, Q + U, Q + V) + aabb(Q + U + V, Q + U, Q + V));// .padded();
	}
	inline tri trans(const matrix& T) const {
		vec3 q = T * Q + T.P();
		vec3 u = T * U;
		vec3 v = T * V;
		return tri(q, u, v, true);
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
	inline float pdf(const ray& r)const {
		hitrec rec;
		if (!hit(r, rec))return 0;
		float S = 0.5f * N.w();
		float NoL = absdot(N, r.D);
		return rec.t * rec.t / (S * NoL);
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
//tri vec3(Q) vec3(U) vec3(V) vec3(N), 4*4*4 = 64B)
//class tri {
//public:
//	tri() {}
//	tri(vec3 _A, vec3 _B, vec3 _C) :Q(_A), U(_B - _A), V(_C - _A), na(norm(cross(U,V))), nb(na), nc(na) {}
//	tri(vec3 _A, vec3 _B, vec3 _C, vec3 na, vec3 nb, vec3 nc) :Q(_A), U(_B - _A), V(_C - _A), na(na), nb(nb), nc(nc) {}
//	tri(vec3 _Q, vec3 _U, vec3 _V, vec3 na, vec3 nb, vec3 nc, bool param) :Q(_Q), U(_U), V(_V), na(na), nb(nb), nc(nc) {}
//
//	inline aabb get_box()const {
//		return (aabb(Q, Q + U, Q + V) + aabb(Q + U + V, Q + U, Q + V));// .padded();
//	}
//	inline tri trans(const matrix& T) const {
//		vec3 q = T * Q + T.P();
//		vec3 u = T * U;
//		vec3 v = T * V;
//		return tri(q, u, v, T * na, T * nb, T * nc, true);
//	}
//
//	inline bool hit(const ray& r, hitrec& rec) const
//	{
//		vec3 pV = cross(r.D, V);
//		float D = dot(U, pV);
//		float iD = 1.f / D;
//		vec3 tV = r.O - Q;
//		vec3 qV = cross(tV, U);
//		float u = dot(tV, pV) * iD;
//		float v = dot(r.D, qV) * iD;
//		float t = dot(V, qV) * iD;
//		if (within(u, 0.f, 1.f) && within(v, 0.f, 1.f - u) && inside(t, eps2, rec.t))
//		{
//			bool face = D > 0;
//			vec3 p = u * Q + v * (Q + U) + (1 - u - v) * (Q + V);
//			vec3 n = norm(u * na + v * nb + (1 - u - v) * nc);
//			rec.N = face ? n : -n;
//			rec.P =  Q + u * U + v * V;
//			rec.t = t;
//			rec.u = u;
//			rec.v = v;
//			rec.face = face;
//			return true;
//		}
//		return false;
//	}
//	inline float pdf(const ray& r)const {
//		return 0;
//	}
//	inline vec3 rand_to(vec3 O) const {
//		float r[2]; rafl_tuple(r);
//		if (r[0] + r[1] > 1.f) {
//			r[0] = 1.f - r[0];
//			r[1] = 1.f - r[1];
//		}
//		vec3 P = Q + r[0] * U + r[1] * V;
//		vec3 L = norm(P - O);
//		return L;
//	}
//	inline vec3 rand_from() const {
//		float r[2]; rafl_tuple(r);
//		if (r[0] + r[1] > 1.f) {
//			r[0] = 1.f - r[0];
//			r[1] = 1.f - r[1];
//		}
//		//vec3 P = Q + r[0] * U + r[1] * V;
//		vec3 L = 0;
//		return L;
//	}
//	vec3 Q, U, V;
//	vec3 na, nb, nc;
//};



