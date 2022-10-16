#pragma once
#include "vec3.h"
#include "onb.h"
/*Geometry terms from
https://typhomnt.github.io/teaching/ray_tracing/pbr_intro/
*/
inline float DGGX(float NoH, float ro)
{
	float a = ro * ro;
	float a2 = a * a;
	float NoH2 = NoH * NoH;
	float nom = a2;
	float denom = (NoH2 * (a2 - 1.0) + 1.0);
	denom = pi * denom * denom;
	return nom / denom;
}

inline float GGX0(float NoV, float ro)
{
	float r = (ro + 1.0);
	float k = (r * r) / 8.0;

	float nom = NoV;
	float denom = NoV * (1.0 - k) + k;

	return nom / denom;
}
inline float GGX1(float NoV, float NoL, float ro)
{
	float ggx2 = GGX0(NoV, ro);
	float ggx1 = GGX0(NoL, ro);
	return ggx1 * ggx2;
}

/*
From:
https://schuttejoe.github.io/post/ggximportancesamplingpart1/
*/
inline float DGGX2(float NoH, float a)
{
	float a2 = a * a;
	float NoH2 = NoH * NoH;
	float nom = a2;
	float denom = (NoH2 * (a2 - 1.0) + 1.0);
	denom = pi * denom * denom;
	return nom / denom;
}
inline float GGX2(float NoV, float NoL, float a)
{
	float a2 = a * a;
	float denomA = NoV * sqrtf(a2 + (1.0f - a2) * NoL * NoL);
	float denomB = NoL * sqrtf(a2 + (1.0f - a2) * NoV * NoV);
	return 2.0f * NoL * NoV / (denomA + denomB);
}
inline float FRES(float NoV, float F0)
{
	return F0 + (1.0 - F0) * pow5(1.0f - NoV);
}

inline vec3 FRES(float NoV, vec3 F0)
{
	return F0 + (1.0 - F0) * pow5(1.0f - NoV);
}

