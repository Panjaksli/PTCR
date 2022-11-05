#pragma once

#include "pdf.h"
#include "obj_list.h"
#include "camera.h"

struct options {
	float res_scale = 1.f;
	float res_rate = 1.f;
	float p_life = 0.9f;
	float i_life = 1.f / 0.9f;
	int bounces = 5;
	int samples = 2;
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
	//definitions
	inline vec3 raycol_at(const ray& r)const {
		hitrec rec; matrec mat;
		if (!world.hit(r, rec)) return 0;
		world.materials[rec.mat].sample(r, rec, mat);
		return mat.diff + mat.spec + mat.emis;
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
	inline ray sa_li_sun(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		cos_pdf cosine(mat.N, mat.dr);
		lig_pdf lights(world, P);
		sun_pdf sun(sun_pos, P);
		mix_pdf<sun_pdf, lig_pdf> ill(sun, lights);
		mix_pdf<cos_pdf, mix_pdf<sun_pdf, lig_pdf>> mix(cosine, ill);
		ray R(P, mix.generate());
		p1 = cosine.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	inline ray sa_li(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		cos_pdf cosine(mat.N, mat.dr);
		lig_pdf lights(world, P);
		mix_pdf<cos_pdf, lig_pdf> mix(cosine, lights);
		ray R(P, mix.generate());
		p1 = cosine.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	inline ray sa_sun(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		cos_pdf cosine(mat.N, mat.dr);
		sun_pdf sun(sun_pos, P);
		mix_pdf<cos_pdf, sun_pdf> mix(cosine, sun);
		ray R(P, mix.generate());
		p1 = cosine.value(R.D);
		p2 = mix.value(R.D);
		return R;
	}
	inline ray sa_none(const matrec& mat, const vec3& P, float& p1, float& p2) const
	{
		ray R(P, mat.dr);
		p1 = 1.f;
		p2 = 1.f;
		return R;
	}
	inline vec3 path_trace(const ray& r, int depth)const {
		hitrec rec;
		if (depth <= -1)return 0;
		if (!world.hit(r, rec)) return sky(r.D);
		return sample_recursive(r, rec, depth);
	}

	inline vec3 raycol(const ray& r)const {
		hitrec rec; vec3 col;
		int depth = opt.bounces;
		if (!world.hit(r, rec)) return opt.samples * sky(r.D);
		for (int i = 0; i < opt.samples; i++)
		{
			col += opt.recur ? sample_recursive(r, rec, depth) : sample_iterative(r, rec, depth);
		}
		return col;
	}
	inline vec3 sky(vec3 V) const
	{
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
	//declarations
	inline vec3 sample_recursive(const ray& r, const hitrec& rec, int depth) const;
	inline vec3 sample_iterative(const ray& r, const hitrec& rec, int depth) const;
	obj_id get_id(const ray& r, hitrec& rec) const;
	obj_id get_id(float py, float px, matrix& T) const;
	void focus_cam();
	void set_trans(obj_id id, const matrix& T);
	void render();
	void screenshot() const;
public:
	camera cam;
	obj_list world;
	matrix sun_pos;
	options opt;
	inline void* image() {
		return cam.CCD.disp.data();
	}
};

inline vec3 scene::sample_recursive(const ray& r, const hitrec& rec, int depth) const {
	matrec mat; vec3 aten;
	float pt = rafl();
	if (pt >= opt.p_life)return 0;
	world.materials[rec.mat].sample(r, rec, mat);
	bool spec = not0(mat.spec), diff = not0(mat.diff);
	vec3 sd_mul = spec + diff;
	//Recursive formulation, using modified propability for specular/diffuse, increased specular variance
	if (opt.p_mode) {
		bool sd = rafl() < 0.5f;
		if ((sd || !diff) && spec) {
			vec3 P = rec.P + rec.N * eps;
			aten += mat.spec * path_trace(ray(P, mat.sr), depth - 1);
		}
		else if ((!sd || !spec) && diff)
		{
			vec3 P = rec.P + rec.N * eps;
			ray R; float p1, p2;
			if (opt.li_sa && opt.sun_sa) R = sa_li_sun(mat, P, p1, p2);
			else if (opt.sun_sa) R = sa_sun(mat, P, p1, p2);
			else if (opt.li_sa) R = sa_li(mat, P, p1, p2);
			else R = sa_none(mat, P, p1, p2);
			if (p1 > 0) aten += (p1 / p2) * mat.diff * path_trace(R, depth - 1);
		}
		return (sd_mul * aten + mat.emis) * opt.i_life;
	}
	//Shoots extra ray for specular -> pow2 growth of ray count, lowest variance
	else {
		if (spec) {
			vec3 P = rec.P + rec.N * eps;
			aten += mat.spec * path_trace(ray(P, mat.sr), depth - 1);
		}
		if (diff)
		{
			vec3 P = rec.P + rec.N * eps;
			ray R; float p1, p2;
			if (opt.li_sa && opt.sun_sa) R = sa_li_sun(mat, P, p1, p2);
			else if (opt.sun_sa) R = sa_sun(mat, P, p1, p2);
			else if (opt.li_sa) R = sa_li(mat, P, p1, p2);
			else R = sa_none(mat, P, p1, p2);
			if (p1 > 0) aten += (p1 / p2) * mat.diff * path_trace(R, depth - 1);
		}
		return (aten + mat.emis) * opt.i_life;
	}
}
inline vec3 scene::sample_iterative(const ray& sr, const hitrec& srec, int depth) const {
	vec3 col(0), aten(1.f); ray r = sr;
	for (int i = 0; i < depth + 1; i++)
	{
		if (rafl() >= opt.p_life || near0(aten)) break;
		aten *= opt.i_life;
		hitrec rec; matrec mat;
		if (i == 0) rec = srec;
		else if (!world.hit(r, rec)) {
			col += aten * sky(r.D); 
			break;
		}
		world.materials[rec.mat].sample(r, rec, mat);
		col += mat.emis * aten;

		if(opt.p_mode){
		aten *= 2.f;
		bool spec = not0(mat.spec), diff = not0(mat.diff);
		bool sd = rafl() < 0.5f;
		if (spec && sd)
		{
			vec3 P = rec.P + rec.N * eps;
			aten *= mat.spec;
			r = ray(P, mat.sr);
		}
		else if (diff && !sd)
		{
			float p1, p2;
			vec3 P = rec.P + rec.N * eps;
			if (opt.li_sa && opt.sun_sa) r = sa_li_sun(mat, P, p1, p2);
			else if (opt.sun_sa) r = sa_sun(mat, P, p1, p2);
			else if (opt.li_sa) r = sa_li(mat, P, p1, p2);
			else r = sa_none(mat, P, p1, p2);
			if (p1 > 0) aten *= (p1 / p2) * mat.diff;
			else break;
		}
		else break;
		}


		else {
			bool spec = not0(mat.spec), diff = not0(mat.diff);
			aten *= spec + diff;
			bool sd = rafl() < 0.5f;
			if ((sd || !diff) && spec)
			{
				vec3 P = rec.P + rec.N * eps;
				aten *= mat.spec;
				r = ray(P, mat.sr);
			}
			else if ((!sd || !spec) && diff)
			{
				float p1, p2;
				vec3 P = rec.P + rec.N * eps;
				if (opt.li_sa && opt.sun_sa) r = sa_li_sun(mat, P, p1, p2);
				else if (opt.sun_sa) r = sa_sun(mat, P, p1, p2);
				else if (opt.li_sa) r = sa_li(mat, P, p1, p2);
				else r = sa_none(mat, P, p1, p2);
				if (p1 > 0) aten *= (p1 / p2) * mat.diff;
				else break;
			}
			else break;
		}

	}
	return col;
}


