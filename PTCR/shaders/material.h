#pragma once
#include "vec3.h"
#include "ray.h"
#include "obj.h"
#include "albedo.h"
#include "samplers.h"
#include "shading.h"

struct matrec {
public:
	vec3 N;	//Normal from normal map
	vec3 dr, sr; //Ray dir: diffuse, specular
	vec3 diff, spec, emis; //Color: diffuse, specular, emissive
};

enum mat_enum {
	mat_lam, mat_mir, mat_ggx, mat_vnd, mat_uli, mat_dli
};
namespace material {
	inline void lambert(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		vec3 N = normal_map(rec.N, nor);
		vec3 V = -r.D;
		vec3 L = onb(N).world(sa_cos());
		mat.N = N;
		mat.dr = L;
		mat.diff = rgb;
	}

	inline void mirror(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		vec3 N = normal_map(rec.N, nor);
		vec3 V = r.D;
		vec3 L = reflect(V, N);
		float a = mer.z * mer.z;
		L = norm(mix(L, ra_hem(L), a));
		mat.N = N;
		mat.sr = L;
		mat.spec = rgb;
	}
	inline void ggx(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float mu = mer.x;
		float em = mer.y;
		float ro = mer.z;
		float a = ro * ro;
		vec3 N = normal_map(rec.N, nor);
		vec3 V = -r.D;
		//it's beneficial to stay in world space performance wise
		vec3 H = onb(N).world(sa_ggx(a));
		vec3 L = reflect(r.D, H);
		float NoV = dot(N, V);
		float NoL = dot(N, L);
		float NoH = dot(N, H);
		float HoL = dot(H, L);
		vec3 F(0);
		if(NoV > 0 && NoL > 0 && NoH > 0 && HoL > 0){
		vec3 F0 = mix(0.04f, rgb, mu);
		F = FRES(HoL, F0);
		float G = GGX(NoL, NoV, a);
		float W = HoL / (NoV * NoH);
		mat.spec = F * G * W;
		mat.sr = L;
		}
		mat.N = N;
		mat.diff = (1 - F) * (1 - mu) * rgb;
		mat.dr = onb(N).world(sa_cos());
		mat.emis = rgb * em;
	}
	inline void vndf(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float mu = mer.x;
		float em = mer.y;
		float ro = mer.z;
		float a = ro * ro;
		vec3 N = normal_map(rec.N, nor);
		onb n = onb(N);
		vec3 V = n.local(-r.D);
		//it's beneficial to stay in local space performance wise
		vec3 H = sa_vndf(V, a);
		vec3 L = reflect(-V, H);
		float NoV = V.z;
		float NoL = L.z;
		float HoL = dot(H, L);
		vec3 F(0);
		if (NoV > 0 && NoL > 0 && HoL > 0) {
			vec3 F0 = mix(0.04f, rgb, mu);
			F = FRES(HoL, F0);
			float G = GGX(NoL, NoV, a);
			float W = MGGX(NoV, a);
			mat.spec = F * G / W;
			mat.sr = n.world(L);
		}
		mat.N = N;
		mat.diff = (1 - F) * (1 - mu) * rgb;
		mat.dr = n.world(sa_cos());
		mat.emis = rgb * em;
	}

	inline void uni_li(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		mat.N = rec.N;
		mat.emis = mer.y * rgb;
	}

	inline void dir_li(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		mat.N = rec.N;
		mat.emis = rec.face * mer.y * rgb;
	}
}
class mat_var {
public:
	mat_var(const albedo& tex, mat_enum type) :tex(tex), type(type) {}

	inline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		switch (type) {
		case mat_lam: return material::lambert(r, rec, tex, mat);
		case mat_mir: return material::mirror(r, rec, tex, mat);
		case mat_ggx: return material::ggx(r, rec, tex, mat);
		case mat_vnd: return material::vndf(r, rec, tex, mat);
		case mat_uli: return material::uni_li(r, rec, tex, mat);
		case mat_dli: return material::dir_li(r, rec, tex, mat);
		default: return;
		};
	}
	albedo tex;
	mat_enum type;
};
