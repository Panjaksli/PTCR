#pragma once

#include "pdf.h"
#include "obj_list.h"
#include "camera.h"

struct options {
	float res_scale = 1.f;
	float res_rate = 1.f;
	float spec_rate = 1.f;
	int bounces = 5;
	int samples = 2;
	bool sun_sa = 1;
	bool li_sa = 1;
	bool dbg_at = 0;
	bool dbg_n = 0;
	bool dbg_uv = 0;
	bool dbg_t = 0;
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
		rec.mat->sample(r, rec, mat);
		return mat.diff + mat.spec + mat.emis;
	}
	inline vec3 raycol_n(const ray& r)const {
		hitrec rec; matrec mat;
		if (!world.hit(r, rec)) return 0;
		rec.mat->sample(r, rec, mat);
		return (mat.N + 1.f) * 0.5f;
	}
	inline vec3 raycol_uv(const ray& r)const {
		hitrec rec;
		if (!world.hit(r, rec)) return 0;
		return vec3(rec.u, 0, rec.v);
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
	inline vec3 raycol(const ray& r)const {
		hitrec rec; vec3 col;
		int depth = opt.bounces;
		if (!world.hit(r, rec)) return opt.samples * sky(r.D);
		for (int i = 0; i < opt.samples; i++)
		{
			col += sample(r, rec, depth);
		}
		return col;
	}

	inline vec3 path_trace(const ray& r, int depth)const {
		hitrec rec;
		if (depth <= 0)return 0;
		if (!world.hit(r, rec)) return sky(r.D);
		return sample(r, rec, depth);
	}

	

	inline void* image() {
		return cam.CCD.disp.data();
	}
	

	//declarations
	inline vec3 sample(const ray& r, const hitrec& rec, int depth) const;
	obj_id get_id(const ray& r, hitrec& rec) const;
	obj_id get_id(float py, float px, matrix& T) const;
	void focus_cam();
	void set_trans(obj_id id, const matrix& T);
	void render();
	void screenshot() const;
private:
	inline vec3 sky(vec3 V) const;
public:
	camera cam;
	obj_list world;
	matrix sun_pos;
	options opt;
};

inline vec3 scene::sample(const ray& r, const hitrec& rec, int depth) const {
	matrec mat; vec3 aten;
	rec.mat->sample(r, rec, mat);
	if (not0(mat.spec) && rafl() < opt.spec_rate)
		aten += (1.f / opt.spec_rate) * mat.spec * path_trace(ray(rec.P, mat.sr), depth - 1);
	if (not0(mat.diff))
	{
		if (opt.li_sa) {
			if (opt.sun_sa) {
				cos_pdf cosine(mat.N, mat.dr);
				lig_pdf lights(world, rec.P);
				sun_pdf sun(sun_pos, rec.P);
				mix_pdf<sun_pdf, lig_pdf> ill(sun, lights);
				mix_pdf<cos_pdf, mix_pdf<sun_pdf, lig_pdf>> mix(cosine, ill);
				ray R(rec.P, mix.generate());
				float p1 = cosine.value(R.D);
				float p2 = mix.value(R.D);
				if (p1 > 0) aten += (p1 / p2) * mat.diff * path_trace(R, depth - 1);
			}
			else {
				cos_pdf cosine(mat.N, mat.dr);
				lig_pdf lights(world, rec.P);
				mix_pdf<cos_pdf, lig_pdf> mix(cosine, lights);
				ray R(rec.P, mix.generate());
				float p1 = cosine.value(R.D);
				float p2 = mix.value(R.D);
				if (p1 > 0) aten += (p1 / p2) * mat.diff * path_trace(R, depth - 1);
			}
		}
		else if (opt.sun_sa) {
			cos_pdf cosine(mat.N, mat.dr);
			sun_pdf sun(sun_pos, rec.P);
			mix_pdf<cos_pdf, sun_pdf> mix(cosine, sun);
			ray R(rec.P, mix.generate());
			float p1 = cosine.value(R.D);
			float p2 = mix.value(R.D);
			if (p1 > 0) aten += (p1 / p2) * mat.diff * path_trace(R, depth - 1);
		}
		else aten += mat.diff * path_trace(ray(rec.P, mat.dr), depth - 1);
	}
	return aten + mat.emis;
}


inline vec3 scene::sky(vec3 V) const
{
	vec3 A = sun_pos * vec3(0, 1, 0);
	float dp = posdot(V, A);
	float dp2 = 0.5f * (1.f + dot(V, A));
	float ip = 1.f - fabsf(A.y);
	float mp = 0.5f * (A.y + 1.f);
	vec3 skycol = pow2n(mp, 1) * mix(vec3(0.0529, 0.1632, 0.445), vec3(0.152, 0.0384, 0.05), ip * ip);
	vec3 suncol = mix(vec3(300, 210, 90), vec3(80, 8, 4), ip * ip);
	skycol = mix(skycol * 0.5f, 2.f * skycol, dp2);
	if (dp > 0.985f)
		return mix(skycol, suncol, pow2n(dp, 10));
	else
		return skycol;
}
