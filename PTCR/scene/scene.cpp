#include <omp.h>
#include "scene.h"

void scene::focus_cam() {
	if (cam.autofocus)
	{
		ray r(cam.focus_ray());
		cam.foc_t = 0.5f * (cam.foc_t + fminf(closest_t(r), 1000.f));
	}
}
obj_id scene::get_id(const ray& r, hitrec& rec) const
{
	return  world.get_id(r, rec);
}

obj_id scene::get_id(float py, float px, matrix& T) const
{
	ray r(cam.raycast(py * opt.res_scale + 0.5, px * opt.res_scale + 0.5));
	hitrec rec;
	obj_id id = get_id(r, rec);
	world.get_trans(id, T);
	if (id.type == o_bla)
		T = sun_pos;
	return id;
}

void scene::set_trans(obj_id id, const matrix& T) {
	if (id.type == o_bla)
	{
		sun_pos = T;
		sun_pos.set_P(vec3());
	}
	else
	{
		world.set_trans(id, T);
	}
}

void scene::render() {
	float invs = 1.f / fmaxf(opt.samples, 1);
	focus_cam();
	if (cam.moving)cam.CCD.t = 0.f;
	if (world.lights.size() <= 0)
		opt.li_sa = 0;
	cam.CCD.dt(opt.samples);
#if DEBUG
	invs = opt.dbg_at || opt.dbg_n || opt.dbg_uv || opt.dbg_t ? 1.f : invs;
#endif
#pragma omp parallel for schedule(dynamic,100)
	for (int k = 0; k < cam.CCD.n; k++)
	{
		if (cam.moving) cam.CCD.clear(k);
		if (rafl() >= opt.res_rate)continue;
		int i = k / cam.w;
		int j = k % cam.w;
		vec3 col(0);
		float jit[2]; rafl_tuple(jit);
		float pxr = j + jit[0] - 0.5f;
		float pyr = i + jit[1] - 0.5f;
		ray r(cam.raycast(pyr, pxr));
#if DEBUG
		if (opt.dbg_at)		 col = raycol_at(r);
		else if (opt.dbg_n)  col = raycol_n(r);
		else if (opt.dbg_uv) col = raycol_uv(r);
		else if (opt.dbg_t)  col = raycol_t(r);
		else
#endif
			col = raycol(r);
		cam.pixel(i, j, col * invs);
	}
	cam.moving = 0;
}
void scene::screenshot()const {

}