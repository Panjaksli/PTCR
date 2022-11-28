#pragma once
#include "vec3.h"

struct matrix {
	matrix(vec3 P = 0, vec3 _A = 0) : A(_A) {
		rot();
		set_P(P);
	}
	inline vec3 P() const {
		return vec3(R[0].w(), R[1].w(), R[2].w());
	}
	inline void set_P(vec3 P)
	{
		R[0].xyz[3] = P.x();
		R[1].xyz[3] = P.y();
		R[2].xyz[3] = P.z();
	}
	inline void add_P(vec3 P)
	{
		R[0].xyz[3] += P.x();
		R[1].xyz[3] += P.y();
		R[2].xyz[3] += P.z();
	}

	inline void set_A(vec3 _A)
	{
		A = _A;
		rot();
	}
	inline void add_A(vec3 _A)
	{
		A += _A;
		rot();
	}
	inline void set(vec3 P, vec3 _A)
	{
		set_P(P);
		set_A(_A);
	}
	inline void add(vec3 P, vec3 _A)
	{
		add_P(P);
		add_A(_A);
	}
	inline void rot()
	{
		A -= pi2 * floor(A * ipi2);
		vec3 sin_A = sin(A);
		vec3 cos_A = cos(A);
		R[0].xyz[0] = cos_A.x() * cos_A.y();
		R[0].xyz[1] = cos_A.x() * sin_A.y() * sin_A.z() - sin_A.x() * cos_A.z();
		R[0].xyz[2] = cos_A.x() * sin_A.y() * cos_A.z() + sin_A.x() * sin_A.z();
		R[1].xyz[0] = sin_A.x() * cos_A.y();
		R[1].xyz[1] = sin_A.x() * sin_A.y() * sin_A.z() + cos_A.x() * cos_A.z();
		R[1].xyz[2] = sin_A.x() * sin_A.y() * cos_A.z() - cos_A.x() * sin_A.z();
		R[2].xyz[0] = -sin_A.y();
		R[2].xyz[1] = cos_A.y() * sin_A.z();
		R[2].xyz[2] = cos_A.y() * cos_A.z();
	}
	inline vec3 operator*(vec3 u) const
	{
		vec3 v;
		v.xyz[0] = dot4(u, R[0]);
		v.xyz[1] = dot4(u, R[1]);
		v.xyz[2] = dot4(u, R[2]);
		v.xyz[3] = u.w();
		return v;
	}
	void print() {
		for (int i = 0; i < 3; i++)
			R[i].print4();
	}
	vec3 R[3], A;
};
inline matrix operator+(matrix T1,const matrix& T2) {
	T1.add_A(T2.A);
	T1.add_P(T2.P());
	T1.rot();
	return T1;
}
inline matrix operator-(matrix T1, const matrix& T2) {
	T1.add_A(-T2.A);
	T1.add_P(-T2.P());
	T1.rot();
	return T1;
}