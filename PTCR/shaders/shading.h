#pragma once
#include "vec3.h"

/*
From:
https://schuttejoe.github.io/post/ggximportancesamplingpart1/
*/

inline float DGGX(float NoH, float a)
{
	float a2 = a * a;
	float d = NoH * NoH * (a2 - 1.0f) + 1.0f;
	return ipi * a2 / (d * d);
}
inline float MGGX(float NoV, float a)
{
	float a2 = a * a;
	float d = sqrtf(a2 + (1.f + a2) * NoV * NoV) + NoV;
	return 2.f * NoV / d;

}
inline float GGX(float NoL, float NoV, float a)
{
	float a2 = a * a;
	float d1 = NoV * sqrtf(a2 + (1.0f - a2) * NoL * NoL);
	float d2 = NoL * sqrtf(a2 + (1.0f - a2) * NoV * NoV);
	return 2.0f * NoL * NoV / (d1 + d2);
}
inline float FRES(float NoV, float F0)
{
	return F0 + (1.0 - F0) * pow5(1.0f - NoV);
}

inline vec3 FRES(float NoV, vec3 F0)
{
	return F0 + (1.0 - F0) * pow5(1.0f - NoV);
}

inline float FRES(float NoV, float F0, float power)
{
	return F0 + (1.0 - F0) * powf(1.0f - NoV,power);
}

inline vec3 FRES(float NoV, vec3 F0, float power)
{
	return F0 + (1.0 - F0) * powf(1.0f - NoV,power);
}