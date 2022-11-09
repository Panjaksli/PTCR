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
	vec3 P; //Adjusted hitpoint
	vec3 L; //Ray dir: diffuse, specular
	vec3 scat, emis; //Color
	uchar sd = 0; // specular / diff
};

enum mat_enum {
	mat_mix, mat_ggx, mat_vnd, mat_uli, mat_dli
};
namespace material {

	__forceinline void mix(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		//simple mix of lambertian and mirror reflection + emission
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		vec3 N = normal_map(rec.N, nor);
		if (rafl() < mer.x) {
			vec3 L = reflect(r.D, N);
			L = norm(mix(L, sa_hem(L), mer.z * mer.z));
			mat.L = L;
			mat.scat = rgb;
			mat.sd = 1;
		}
		else {
			mat.L = onb(N).world(sa_cos());
			mat.scat = rgb;
			mat.scat = rgb;
			mat.sd = 2;
		}
		mat.N = N;
		mat.P = rec.P + rec.N * eps;
		mat.emis = mer.y * rgb;
		mat.sd *= not0(mat.scat);
	}
	__forceinline void ggx(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float mu = mer.x;
		float em = mer.y;
		float ro = mer.z;
		float a = ro * ro;
		vec3 N = normal_map(rec.N, nor);
		onb n = onb(N);
		vec3 V = -r.D;
		//it's beneficial to stay in world space performance wise
		vec3 H = n.world(sa_ggx(a));
		vec3 L = reflect(r.D, H);
		float NoV = dot(N, V);
		float NoL = dot(N, L);
		float NoH = dot(N, H);
		float HoL = dot(H, L);
		vec3 F = mix(0.04f, rgb, mu);
		bool correct = NoV > 0 && HoL > 0 && NoL > 0 && NoH > 0;
		if (correct) F = FRES(HoL, F);
		float sd = max(F);
		bool spec = rafl() < sd;
		if (spec && correct) {
			float G = GGX(NoL, NoV, a);
			float W = HoL / (NoV * NoH);
			mat.scat = F * (G * W) / sd;
			mat.L = L;
			mat.sd = 1;
		}
		else {
			mat.scat = ((1.f - F) * (1.f - mu)) * rgb / (1.f - sd);
			mat.L = n.world(sa_cos());
			mat.sd = 2;
		}
		mat.N = N;
		mat.P = rec.P + rec.N * eps;
		mat.emis = rgb * em;
		mat.sd *= not0(mat.scat);
	}
	__forceinline void vndf(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float mu = mer.x;
		float em = mer.y;
		float ro = mer.z;
		float a = ro * ro;
		vec3 N = normal_map(rec.N, nor);
		//todo : if NoV < 0 shouldnt be no light visible ???
		onb n = onb(N);
		vec3 V = n.local(-r.D);
		//it's beneficial to stay in local space performance wise
		//this sampling strategy is supposedly better, but performance is WAYY worse than GGX, and fireflies are still occuring the same amount.
		vec3 H = sa_vndf(V, a);
		vec3 L = reflect(-V, H);
		float NoV = V.z;
		float NoL = L.z;
		float HoL = dot(H, L);
		vec3 F = mix(0.04f, rgb, mu);
		bool correct = HoL > 0 && NoV > 0 && NoL > 0;
		if (correct) F = FRES(HoL, F);
		float sd = max(F);
		bool spec = rafl() < sd;
		if (spec && correct) {
			mat.scat = F * VNDF_GGX(NoL, NoV, a) / sd;
			mat.L = n.world(L);
			mat.sd = 1;
		}
		else {
			mat.scat = ((1.f - F) * (1.f - mu)) * rgb / (1 - sd);
			mat.L = n.world(sa_cos());
			mat.sd = 2;
		}
		mat.N = N;
		mat.P = rec.P + rec.N * eps;
		mat.emis = rgb * em;
		mat.sd *= not0(mat.scat);
	}

	__forceinline void uni_li(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		mat.emis = mer.y * rgb;
	}

	__forceinline void dir_li(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		mat.emis = rec.face * mer.y * rgb;
	}
}
class mat_var {
public:
	mat_var(const albedo& tex, mat_enum type) :tex(tex), type(type) {}

	__forceinline void sample(const ray& r, const hitrec& rec, matrec& mat)const {
		switch (type) {
		case mat_mix: return material::mix(r, rec, tex, mat);
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
