#pragma once
#include "vec3.h"

class ray {
public:
	ray() {}
	ray(vec3 _O, vec3 _D) :O(_O), D(_D), iD(1.f / D) {}
	ray(vec3 _O, vec3 _D, bool normalize) :O(_O), D(norm(_D)), iD(1.f / D) {}
	__forceinline vec3 at(float t) const
	{
		return O + t * D;
	}
	vec3 O, D, iD;
};
