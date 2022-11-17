#pragma once

#include "pdf.h"
#include "obj_list.h"
#include "camera.h"

struct options {
	options() {}
	float res_scale = 1.f;
	float res_rate = 1.f;
	float p_life = 0.5f;
	float i_life = 2.f;
	int bounces = 5;
	int samples = 2;
	bool sky = 1;
	bool sun_sa = 1;
	bool li_sa = 1;
	bool dbg_at = 0;
	bool dbg_n = 0;
	bool dbg_uv = 0;
	bool dbg_t = 0;
	bool recur = 0;
	bool p_mode = 0;
};

class scene {
public:
	scene() {}
	scene(camera cam, obj_list world = {}) :cam(cam), world(world) {}
	scene(float fov, uint w, uint h, obj_list world = {}) :cam(fov, w, h), world(world) {}
	camera cam;
	obj_list world;
	matrix sun_pos;
	options opt;


	obj_id get_id(const ray& r, hitrec& rec) const;
	obj_id get_id(float py, float px, matrix& T) const;
	void cam_autofocus();
	void cam_manufocus(float py = 0, float px = 0);
	void set_trans(obj_id id, const matrix& T);
	void render(uint* disp, uint pitch);
private:
	inline vec3 raycol_at(const ray& r)const {
		hitrec rec; matrec mat;
		if (!world.hit(r, rec)) return 0;
		world.materials[rec.mat].sample(r, rec, mat);
		return mat.scat + mat.emis;
	}
	inline vec3 raycol_n(const ray& r)const {
		hitrec rec; matrec mat;
		if (!world.hit(r, rec)) return 0;
		world.materials[rec.mat].sample(r, rec, mat);
		vec3 col = (mat.N + 1.f) * 0.5f;
		return col * col;
	}
	inline vec3 raycol_uv(const ray& r)const {
		hitrec rec;
		if (!world.hit(r, rec)) return 0;
		return vec3(rec.u, 0, rec.v) * vec3(rec.u, 0, rec.v);
	}
	inline vec3 raycol_t(const ray& r)const {
		float t = closest_t(r);
		return min(vec3(t * 0.1f, t * 0.01f, t * 0.001f), 1);
	}
	inline float closest_t(const ray& r) const {
		hitrec rec;
		world.hit(r, rec);
		return rec.t;
	}

	__forceinline ray sa_diff(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		if (opt.li_sa && opt.sun_sa) return sa_li_sun(mat, P, p1, p2);
		else if (opt.sun_sa) return sa_sun(mat, P, p1, p2);
		else if (opt.li_sa) return sa_li(mat, P, p1, p2);
		else return sa_none(mat, P, p1, p2);
	}
	__forceinline ray sa_fog(const vec3& P, float& p1, float& p2) const
	{
		sph_pdf fog;
		sun_pdf sun(sun_pos, P);
		mix_pdf<sun_pdf, sph_pdf> mix(sun, fog);
		ray R(P, mix.generate());
		p1 = fog.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	__forceinline ray sa_li_sun(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		cos_pdf cosine(mat.N, mat.L);
		lig_pdf lights(world, P);
		sun_pdf sun(sun_pos, P);
		mix_pdf<sun_pdf, lig_pdf> ill(sun, lights);
		mix_pdf<cos_pdf, mix_pdf<sun_pdf, lig_pdf>> mix(cosine, ill);
		ray R(P, mix.generate());
		p1 = cosine.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	__forceinline ray sa_li(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		cos_pdf cosine(mat.N, mat.L);
		lig_pdf lights(world, P);
		mix_pdf<cos_pdf, lig_pdf> mix(cosine, lights);
		ray R(P, mix.generate());
		p1 = cosine.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	__forceinline ray sa_sun(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		cos_pdf cosine(mat.N, mat.L);
		sun_pdf sun(sun_pos, P);
		mix_pdf<cos_pdf, sun_pdf> mix(cosine, sun);
		ray R(P, mix.generate());
		p1 = cosine.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	__forceinline ray sa_none(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		ray R(P, mat.L);
		p1 = 1.f;
		p2 = 1.f;
		return R;
	}
	__forceinline vec3 path_trace(const ray& r, int depth)const {
		hitrec rec;
		if (depth <= -1)return 0;
		if (!world.hit(r, rec)) return sky(r.D);
		if (rafl() >= opt.p_life)return 0;
		return opt.i_life * sample_recursive(r, rec, depth);
	}

	__forceinline vec3 raycol(const ray& r)const {
		hitrec rec; vec3 col;
		int depth = opt.bounces;
		if (!world.hit(r, rec)) return opt.samples * sky(r.D);
		for (int i = 0; i < opt.samples; i++)
		{
			col += opt.recur ? sample_recursive(r, rec, depth) : sample_iterative(r, rec, depth);
		}
		return col;
	}
	__forceinline vec3 sky(vec3 V) const
	{
		if (!opt.sky)return 0;
		vec3 A = sun_pos * vec3(0, 1, 0);
		float dp = posdot(V, A);
		float dp2 = 0.5f * (1.f + dot(V, A));
		float ip = 1.f - fabsf(A.y);
		float mp = 0.5f * (A.y + 1.f);
		vec3 skycol = (pow2n(mp, 2) + 0.001f) * mix(vec3(0.0529, 0.1632, 0.445), vec3(0.152, 0.0384, 0.05), ip * ip);
		vec3 suncol = mix(vec3(300, 210, 90), vec3(80, 8, 4), ip * ip);
		skycol = mix(skycol * 0.5f, 2.f * skycol, dp2);
		if (dp > 0.985f)
			return mix(skycol, suncol, pow2n(dp, 10));
		else
			return skycol;
	}

	__forceinline vec3 sample_recursive(const ray& r, const hitrec& rec, int depth) const;
	__forceinline vec3 sample_iterative(const ray& r, const hitrec& rec, int depth) const;

};

__forceinline  vec3 scene::sample_recursive(const ray& r, const hitrec& rec, int depth) const {
	matrec mat; vec3 aten;
	world.materials[rec.mat].sample(r, rec, mat);
	if (mat.sd) {
		if (mat.sd == 1)
			aten += mat.scat * path_trace(ray(mat.P, mat.L), depth - 1);
		else
		{
			ray R; float p1, p2;
			R = sa_diff(mat, mat.P, p1, p2);
			if (p1 > 0) aten += (p1 / p2) * mat.scat * path_trace(R, depth - 1);
		}
		return aten + mat.emis;
	}
	else return mat.emis;
}
__forceinline  vec3 scene::sample_iterative(const ray& sr, const hitrec& srec, int depth) const {
	vec3 col(0), aten(1.f); ray r = sr;
	for (int i = 0; i < depth + 1; i++)
	{
		hitrec rec; matrec mat;
		if (i == 0) rec = srec;
		else if (!world.hit(r, rec)) return col += aten * sky(r.D);
		else if (rafl() >= opt.p_life) break;
		else aten *= opt.i_life;
		world.materials[rec.mat].sample(r, rec, mat);
		col += mat.emis * aten;
		if (mat.sd)
		{
			if (mat.sd == 1)
				r = ray(mat.P, mat.L);
			else
			{
				float p1, p2;
				r = sa_diff(mat, mat.P, p1, p2);
				if (p1 > 0)aten *= (p1 / p2);
				else break;
			}
			aten *= mat.scat;
		}
		else break;
	}
	return col;
}


