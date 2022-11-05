#pragma once
#include "vec3.h"
#include "onb.h"
inline vec3 sa_cos()
{
	float r[2];
	rafl_tuple(r);
	const float phi = pi2 * r[0];
	const float x = fcos(phi) * sqrtf(r[1]);
	const float y = fsin(phi) * sqrtf(r[1]);
	const float z = sqrtf(1.f - r[1]);

	return vec3(x, y, z);
}

inline vec3 sa_uni()
{
	float r[2];
	rafl_tuple(r);
	const float phi = pi2 * r[1];
	const float root = sqrtf(1.f - r[0] * r[0]);
	const float x = fcos(phi) * root;
	const float y = fsin(phi) * root;
	const float z = r[0];

	return vec3(x, y, z);
}
/*
From:
https://schuttejoe.github.io/post/ggximportancesamplingpart1/
*/
inline vec3 sa_ggx2(float a) {
	float r[2];
	rafl_tuple(r);
	float a2 = a * a;
	float theta = facos(sqrtf((1.0f - r[0]) / ((a2 - 1.0f) * r[0] + 1.0f)));
	float phi = pi2 * r[1];
	return vec3(fcos(phi) * fsin(theta), fsin(phi) * fsin(theta), fcos(theta));
}

