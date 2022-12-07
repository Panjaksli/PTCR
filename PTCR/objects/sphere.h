#pragma once
#include "obj.h"

//sphere vec4(x,y,z,r) 16B
class sphere {
public:
	sphere() {}
	sphere(vec3 Q, float r) :Qr(Q, r) {}
	sphere(vec4 _Qr) :Qr(_Qr) {}

	inline bool hit(const ray& r, hitrec& rec) const
	{
		//Quadratic equation, own solution
		vec3 OQ = Qr - r.O;
		float b = dot(r.D, OQ);
		float c = dot(OQ, OQ) - Qr.w() * Qr.w();
		float d2 = b * b - c;
		float d = sqrtf(d2);
		float t1 = b - d;
		float t2 = b + d;
		bool face = c > 0;
		float t = face ? t1 : t2;
		if (inside(t, eps2, rec.t)) {
			vec3 P = r.at(t);
			vec3 N = (P - Qr) / Qr.w();
			rec.N = face ? N : -N;
			rec.P = P;
			rec.t = t;
			rec.u = (fatan2(-N.z(), N.x()) + pi) * ipi2;
			rec.v = facos(-N.y()) * ipi;
			rec.face = face;
			return true;
		}
		return false;
	}

	inline aabb get_box()const {
		return aabb(Qr - Qr.w(), Qr + Qr.w());
	}

	inline sphere trans(const matrix& T) const {
		return sphere(Qr + T.P());
	}

	inline float pdf(const ray& r)const {
		hitrec rec;
		if (!hit(r, rec))return 0;
		if (!rec.face)
		{
			float S = pi4 * Qr.w() * Qr.w();
			float NoL = absdot(rec.N, r.D);
			return rec.t * rec.t / (S * NoL);
		}
		else {
			//propability according to sampled cone, from:
			//https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html#importancesamplingmaterials/randomhemispheresampling
			float theta = sqrtf(1.f - Qr.w() * Qr.w() / (Qr - r.O).len2());
			return  1.f / (pi2 * (1.f - theta));
		}
		return 0;
	}

	inline vec3 rand_to(vec3 O) const {
		vec3 OQ = Qr - O;
		float d2 = OQ.len2();
		float R2 = Qr.w() * Qr.w();
		//if inside, pick uniform coordinate
		if (d2 <= R2)
		{
			vec3 N = sa_sph();
			vec3 P = Qr + N * Qr.w();
			vec3 L = P - O;
			return norm(L);
		}
		//sample cone in direction of sphere, from:
		//https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html#importancesamplingmaterials/randomhemispheresampling
		float r[2]; rafl_tuple(r);
		float z = 1.f + r[1] * (sqrtf(1.f - R2 / d2) - 1.f);
		float phi = pi2 * r[0];
		vec3 xy = sqrtf(1.f - z * z) * cossin(phi);
		return onb(norm(OQ)).world(xy + vec3(0, 0, z));
	}
	inline vec3 rand_from() const {
		vec3 R = sa_sph();
		//vec3 P = Qr + R * Qr.w();
		return onb(R).world(sa_cos());
	}
private:
	vec4 Qr;
};

