#pragma once
#include "obj.h"
//voxel vec4(x,y,z,a) 16B
class voxel {
public:
	voxel() {}
	voxel(vec3 Q, float a) :Qa(Q,a) {}
	voxel(vec4 _Qa) :Qa(_Qa) {}

	inline aabb get_box()const {
		return aabb(A(), B());
	}

	inline voxel trans(const matrix& T) const {
		return voxel(Qa+T.P());
	}
	inline bool hit(const ray& r, hitrec& rec) const
	{
		vec3 t1 = (A() - r.O) * r.iD;
		vec3 t2 = (B() - r.O) * r.iD;
		vec3 tmin = min(t1, t2);
		vec3 tmax = max(t1, t2);
		float mint = max(tmin);
		float maxt = min(tmax);
		bool face = mint > eps;
		float t = face ? mint : maxt;
		if (mint <= maxt && inside(t, eps2, rec.t))
		{
			vec3 P = r.at(t);
			vec3 W = (P - Qa) / Qa.w;
			vec3 N = toint(1.000001f * W);
			vec3 UV = 0.5f * (1.f + W - N);
			rec.N = face ? N : -N;
			rec.P = r.at(t);
			rec.t = t;
			rec.u =  fabs(UV.x - 0.5f) > eps2 ? UV.x : UV.z;
			rec.v =  fabs(UV.y - 0.5f) > eps2 ? UV.y : UV.z;
			rec.face = face;
			return true;
		}
		return false;
	}
	inline float pdf(const ray& r)const {
		hitrec rec;
		if (!hit(r, rec))return 0;
		float S = Qa.w * Qa.w;
		return rec.t * rec.t / S;
	}
	inline vec3 rand_to(vec3 O) const {
		return 0;
	}
	inline vec3 rand_from() const {
		float r[2]; rafl_tuple(r);

		return 0;
	}
	vec4 Qa;
private:
	inline vec3 A()const {
		return Qa - Qa.w;
	}
	inline vec3 B()const {
		return Qa + Qa.w;
	}
};