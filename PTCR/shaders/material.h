#pragma once
#include "vec3.h"
#include "ray.h"
#include "obj.h"
#include "texture.h"
#include "samplers.h"
#include "shading.h"
#if DEBUG
static float spec_filter = 10.f;
#elif
constexpr float spec_filter = 100.f;
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
	lambert(sptr<texture> _tex):_tex(_tex.get()) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		tex_data tex(_tex->sample(rec.u, rec.v));
		vec3 N = normal_map(rec.N, tex.nor);
		vec3 V = -r.D;
		vec3 L = onb(N).local(sa_cos());
		vec3 rgb = tex.col;
		mat.N = N;
		mat.dr = L;
		mat.diff = rgb;
	}
	texture* _tex;
};
class mirror : public material {
public:
	mirror(sptr<texture> _tex) :_tex(_tex.get()) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		tex_data tex(_tex->sample(rec.u, rec.v));
		vec3 N = normal_map(rec.N, tex.nor);
		vec3 V = r.D;
		vec3 L = reflect(V, N);
		vec3 rgb = tex.col;
		mat.N = N;
		mat.sr = L;
		mat.spec = rgb;
	}
	texture* _tex;
};
class pbr : public material {
public:
	pbr(sptr<texture> _tex) :_tex(_tex.get()) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		tex_data tex(_tex->sample(rec.u, rec.v));
		vec3 rgb = tex.col;
		vec3 mer = tex.mer;
		float mu = mer.x;
		float em = mer.y;
		float ro = mer.z, a = ro * ro;
		vec3 N = normal_map(rec.N, tex.nor);
		vec3 V = -r.D;
		vec3 H = onb(N).local(sa_ggx2(a));
		vec3 L = reflect(r.D, H);
		float NoV = dot(N, V);
		float NoL = dot(N, L);
		float NoH = dot(N, H);
		float HoL = dot(H, L);
		float HoV = dot(H, V);
		vec3 F0 = mix(0.04f, rgb, mu);
		vec3 F = 0;
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
	texture* _tex;
};
class uni_light : public material {
public:
	uni_light(sptr<texture> _tex) :_tex(_tex.get()) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		tex_data tex(_tex->sample(rec.u, rec.v));
		vec3 col = tex.col;
		mat.N = rec.N;
		mat.emis = col * col.w;
	}
	texture* _tex;
};

class dir_light : public material {
public:
	dir_light(sptr<texture> _tex) :_tex(_tex.get()) {}
	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const override {
		tex_data tex(_tex->sample(rec.u, rec.v));
		vec3 col = tex.col;
		mat.N = rec.N;
		mat.emis = rec.face * col * col.w;
	}
	texture* _tex;
};

