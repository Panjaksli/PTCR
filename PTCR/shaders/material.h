#pragma once
#include "vec3.h"
#include "ray.h"
#include "obj.h"
#include "albedo.h"
#include "samplers.h"
#include "shading.h"
#if DEBUG
static float spec_filter = 10.f;
#else
constexpr float spec_filter = 10.f;
#endif

struct matrec {
public:
	vec3 N;	//Normal from normal map
	vec3 dr, sr; //Ray dir: diffuse, specular
	vec3 diff, spec, emis; //Color: diffuse, specular, emissive
};

class material {
public:
	virtual ~material() = default;
	inline virtual void sample(const ray& r, const hitrec& rec, matrec& mat)const = 0;
};

class lambert : public material {
public:
	lambert(const albedo& _tex) :tex(_tex) {}
	~lambert()override { tex.clean(); }
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
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
class mirror : public material {
public:
	mirror(const albedo& _tex) :tex(_tex) {}
	~mirror()override { tex.clean(); }
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
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
class pbr : public material {
public:
	pbr(const albedo& _tex) :tex(_tex) {}
	~pbr()override { tex.clean(); }
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
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
class uni_light : public material {
public:
	uni_light(const albedo& _tex) :tex(_tex) {}
	~uni_light()override { tex.clean(); }
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		mat.N = rec.N;
		mat.emis = rgb;
	}
	albedo tex;
};

class dir_light : public material {
public:
	dir_light(const albedo& _tex) :tex(_tex) {}
	~dir_light()override { tex.clean(); }
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		mat.N = rec.N;
		mat.emis = rec.face * rgb;
	}
	albedo tex;
};

