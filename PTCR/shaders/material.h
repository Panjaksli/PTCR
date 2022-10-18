#pragma once
#include "vec3.h"
#include "ray.h"
#include "obj.h"
#include "albedo.h"
#include "samplers.h"
#include "shading.h"
#if DEBUG
extern float spec_filter;
#else
constexpr float spec_filter = 10.f;
#endif

struct matrec {
public:
	vec3 N;	//Normal from normal map
	vec3 dr, sr; //Ray dir: diffuse, specular
	vec3 diff, spec, emis; //Color: diffuse, specular, emissive
};

enum mat_enum {
	m_lam, m_mir, m_pbr, m_uli, m_dli
};

class lambert {
public:
	lambert(const albedo& _tex) :tex(_tex) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		vec3 N = normal_map(rec.N, nor);
		vec3 V = -r.D;
		vec3 L = onb(N).local(sa_cos());
		mat.N = N;
		mat.dr = L;
		mat.diff = rgb;
	}
	albedo tex;
};
class mirror {
public:
	mirror(const albedo& _tex) :tex(_tex) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		vec3 N = normal_map(rec.N, nor);
		vec3 V = r.D;
		vec3 L = reflect(V, N);
		mat.N = N;
		mat.sr = L;
		mat.spec = rgb;
	}
	albedo tex;
};
class pbr {
public:
	pbr(const albedo& _tex) :tex(_tex) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float mu = mer.x;
		float em = mer.y;
		float ro = mer.z;
		float a = ro * ro;
		vec3 N = normal_map(rec.N, nor);
		vec3 V = -r.D;
		vec3 H = onb(N).local(sa_ggx2(a));
		vec3 L = reflect(r.D, H);
		float NoV = dot(N, V);
		float NoL = dot(N, L);
		float NoH = dot(N, H);
		float HoL = dot(H, L);
		float HoV = dot(H, V);
		vec3 F0 = mix(0.04f, rgb, mu);
		vec3 F = 0.f;
		if (NoL > 0.0f && HoL > 0.0f) {
			F = FRES(HoL, F0);
			float G = GGX2(NoV, NoL, a);
			float W = HoV / (NoV * NoH);
			mat.spec = F * fminf(G * W, spec_filter);
			mat.sr = L;
		}
		L = onb(N).local(sa_cos());
		mat.N = N;
		mat.diff = (1 - F) * (1 - mu) * rgb;
		mat.dr = L;
		mat.emis = rgb * em;
	}
	albedo tex;
};
class uni_light {
public:
	uni_light(const albedo& _tex) :tex(_tex) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		mat.N = rec.N;
		mat.emis = rgb;
	}
	albedo tex;
};

class dir_light {
public:
	dir_light(const albedo& _tex) :tex(_tex) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		mat.N = rec.N;
		mat.emis = rec.face * rgb;
	}
	albedo tex;
};

class mat_var {
public:
	mat_var(lambert mat) :l(mat), type(m_lam) {}
	mat_var(mirror mat) :m(mat), type(m_mir) {}
	mat_var(pbr mat) :p(mat), type(m_pbr) {}
	mat_var(uni_light mat) :u(mat), type(m_uli) {}
	mat_var(dir_light mat) :d(mat), type(m_dli) {}
	mat_var(const mat_var& cpy) :type(cpy.type) {
		switch (type) {
		case m_lam: l = cpy.l; break;
		case m_mir:	m = cpy.m; break;
		case m_pbr: p = cpy.p; break;
		case m_uli: u = cpy.u; break;
		case m_dli: d = cpy.d; break;
		default: return;
		};
	}
	~mat_var() {
		switch (type) {
		case m_lam: l.~lambert(); break;
		case m_mir:	m.~mirror(); break;
		case m_pbr: p.~pbr(); break;
		case m_uli: u.~uni_light(); break;
		case m_dli: d.~dir_light(); break;
		default: return;
		};
	}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		switch (type) {
		case m_lam: return l.sample(r, rec, mat);
		case m_mir: return m.sample(r, rec, mat);
		case m_pbr: return p.sample(r, rec, mat);
		case m_uli: return u.sample(r, rec, mat);
		case m_dli: return d.sample(r, rec, mat);
		default: return;
		};
	}


	union {
		lambert l;
		mirror m;
		pbr	p;
		uni_light u;
		dir_light d;
	};
	mat_enum type;
};