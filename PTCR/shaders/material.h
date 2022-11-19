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
	mat_mix, mat_ggx, mat_vnd, mat_lig
};

namespace material {

	__forceinline void mix(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		//simple mix of lambertian and mirror reflection/transmission + emission
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float a = mer.z * mer.z;
		float ir = tex.ir();
		ir = rec.face ? 1.f / ir : ir;
		vec3 N = normal_map(rec.N, nor);
		onb n(N);
		//100% lambertian (reduced variance, since no rough reflections are allowed)
		if (mer.x <= eps && mer.z >= 1.f - eps && rgb.w >= 1.f - eps) {
			mat.L = n.world(sa_cos());
			mat.P = rec.P + rec.N * eps;
			mat.sd = 2;
			mat.N = N;
			mat.scat = rgb;
			mat.emis = mer.y * rgb;
			mat.sd *= not0(mat.scat);
			return;
		}
		vec3 H = n.world(sa_ggx(a));
		float HoV = dot(H, -r.D);
		if (HoV < 0) return;
		float Fr = fres_refl(HoV, ir);
		bool metal = rafl() < mer.x;
		bool opaque = rafl() < rgb.w;
		bool mirror = ir * sqrtf(1.f - HoV * HoV) > 1.f || Fr > rafl();
		if ((opaque && metal) || mirror) {
			mat.P = rec.P + rec.N * eps;
			mat.L = reflect(r.D, H);
			mat.sd = 1;
		}
		else if (!opaque && !mirror) {
			mat.P = rec.P - rec.N * eps;
			mat.L = refract(r.D, H, ir);
			mat.sd = 1;
		}
		else
		{
			mat.L = n.world(sa_cos());
			mat.P = rec.P + rec.N * eps;
			mat.sd = 2;
		}
		mat.N = N;
		mat.scat = rgb;
		mat.emis = mer.y * rgb;
		mat.sd *= not0(mat.scat);
	}
	__forceinline void ggx(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float a = mer.z * mer.z;
		vec3 F0 = mix(0.04f, vec3(rgb, 1), mer.x);
		vec3 N = normal_map(rec.N, nor);
		onb n = onb(N);
		vec3 V = n.local(-r.D);
		float NoV = V.z;
		if (NoV < 0) return;
		vec3 H = sa_ggx(a);
		vec3 L = reflect(-V, H);
		float NoL = L.z;
		float NoH = H.z;
		float HoV = dot(H, V);
		if (HoV < 0 || NoL < 0 || NoH < 0) return;
		vec3 F = fres_spec(HoV, F0);
		bool spec = rafl() < F.w;
		if (spec) {
			float G = GGX(NoL, NoV, a);
			float W = HoV / (NoV * NoH);
			mat.scat = F * G * W / F.w;
			mat.L = n.world(L);
			mat.sd = 1;
		}
		else {
			mat.scat = (1.f - mer.x) * (1.f - F) * rgb / (1.f - F.w);
			mat.L = n.world(sa_cos());
			mat.sd = 2;
		}
		mat.N = N;
		mat.P = rec.P + rec.N * eps;
		mat.emis = rgb * mer.y;
		mat.sd *= not0(mat.scat);
	}
	__forceinline void vndf(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		vec3 nor = tex.nor(rec.u, rec.v);
		float a = mer.z * mer.z;
		vec3 F0 = mix(0.04f, vec3(rgb, 1), mer.x);
		vec3 N = normal_map(rec.N, nor);
		onb n = onb(N);
		vec3 V = n.local(-r.D);
		float NoV = V.z;
		if (NoV < 0) return;
		vec3 H = sa_vndf(V, a);
		vec3 L = reflect(-V, H);
		float NoL = L.z;
		float HoV = dot(H, V);
		if (HoV < 0 || NoL < 0) return;
		vec3 F = fres_spec(HoV, F0);
		bool spec = rafl() < F.w;
		if (spec) {
			mat.scat = F * VNDF_GGX(NoL, NoV, a) / F.w;
			mat.L = n.world(L);
			mat.sd = 1;
		}
		else {
			mat.scat = (1.f - F) * (1.f - mer.x) * rgb / (1.f - F.w);
			mat.L = n.world(sa_cos());
			mat.sd = 2;
		}
		mat.N = N;
		mat.P = rec.P + rec.N * eps;
		mat.emis = rgb * mer.y;
		mat.sd *= not0(mat.scat);
	}

	__forceinline void light(const ray& r, const hitrec& rec, const albedo& tex, matrec& mat) {
		vec3 rgb = tex.rgb(rec.u, rec.v);
		vec3 mer = tex.mer(rec.u, rec.v);
		mat.emis = mer.y * rgb;
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
		case mat_lig: return material::light(r, rec, tex, mat);
		default: return;
		};
	}
	albedo tex;
	mat_enum type;
};
