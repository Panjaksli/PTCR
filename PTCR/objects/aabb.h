#pragma once
#include "vec3.h"
#include "ray.h"

struct aabb {
	aabb() {}
	aabb(const vec3 p1, const vec3 p2) :pmin(min(p1, p2)), pmax(max(p1, p2)) {}
	aabb(const vec3 p1, const vec3 p2, const vec3 p3) :pmin(min(p1, p2, p3)), pmax(max(p1, p2, p3)) {}
	aabb(const vec3 p1, const vec3 p2, const vec3 p3, const vec3 p4) :pmin(min(p1, p2, min(p3, p4))), pmax(max(p1, p2, max(p3, p4))) {}
	aabb(const aabb& box1, const aabb& box2) :pmin(min(box1.pmin, box2.pmin)), pmax(max(box1.pmax, box2.pmax)) {}
	inline aabb operator+(const aabb& box)const {
		aabb box1 = *this;
		box1.join(box);
		return box1;
	}
	inline aabb trans(vec3 P) const {
		return { pmin + P, pmax + P };
	}
	inline void join(const aabb& box) {
		if (neq0(pmin) || neq0(pmax))
		{
			pmin = (min(pmin, box.pmin)), pmax = (max(pmax, box.pmax));
		}
		else
		{
			pmin = box.pmin;
			pmax = box.pmax;
		}
	}
	inline void pad() {
		vec3 delta = (0.5f * eps2) * vec_lt(fabs(pmax - pmin), eps2);
		pmin -= delta;
		pmax += delta;
	}
	inline aabb padded() const {
		aabb nbox = *this;
		nbox.pad();
		return nbox;
	}
	inline vec3 pmid()const {
		return 0.5f * (pmin + pmax);
	}
	inline uchar get_longest_axis() const {
		vec3 x = abs(pmax - pmin);
		uchar axis = 0;
		if (x.y() > x.x()) axis = 1;
		if (x.z() > x[axis]) axis = 2;
		return axis;
	}
	void print()const {
		pmin.print();
		pmax.print();
	}
	/*: 
	Edited from: https://tavianator.com/2011/ray_box.html
	*/
	__forceinline bool hit(const ray& r) const {
		vec3 t1 = (pmin - r.O) * r.iD;
		vec3 t2 = (pmax - r.O) * r.iD;
		vec3 tmin = min(t1, t2);
		vec3 tmax = max(t1, t2);
		float mint = max(tmin);
		float maxt = min(tmax);
		return mint <= maxt && maxt > 0;
	}
	__forceinline float hit(const ray& r, float t) const {
		vec3 t1 = (pmin - r.O) * r.iD;
		vec3 t2 = (pmax - r.O) * r.iD;
		vec3 tmin = min(t1, t2);
		vec3 tmax = max(t1, t2);
		float mint = max(tmin);
		float maxt = min(tmax);
		if (maxt > mint  && maxt > 0 && mint < t) return mint;
		else return infp;
	}
	vec3 pmin, pmax;
};

