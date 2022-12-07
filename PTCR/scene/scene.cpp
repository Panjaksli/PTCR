#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <omp.h>
#include "scene.h"

void scene::cam_autofocus() {
	if (cam.autofocus) {
		ray r(cam.focus_ray());
		float new_t = closest_t(r);
		cam.foc_t = 0.5 * cam.foc_t + 0.5f * new_t;
	}
}
void scene::cam_manufocus(float py, float px) {
	if (!cam.autofocus) {
		ray r(cam.focus_ray(py * opt.res_scale * cam.ih, px * opt.res_scale * cam.iw));
		cam.foc_t = fminf(closest_t(r), 1e6);
		cam.moving = 1;
	}
}
obj_id scene::get_id(const ray& r, hitrec& rec) const
{
	return  world.get_id(r, rec);
}

obj_id scene::get_id(float py, float px, matrix& T) const
{
	ray r(cam.focus_ray(py * opt.res_scale * cam.ih, px * opt.res_scale * cam.iw));
	hitrec rec;
	obj_id id = get_id(r, rec);
	world.get_trans(id, T);
	if (id.type == o_bla)
		T = sun_pos;
	return id;
}

void scene::set_trans(obj_id id, const matrix& T, uint node_size) {
	if (id.type == o_bla)
	{
		//if nothing hit, transform sky
		sun_pos = T;
		sun_pos.set_P(vec3());
	}
	else
	{
		world.set_trans(id, T, node_size);
	}
}

void scene::Render(uint* disp, uint pitch) {
	//write straight to texture (tiny bit less overhead)
	cam.CCD.set_disp(disp, pitch);
	float invs = 1.f / fmaxf(opt.samples, 1);
	cam_autofocus();
	if (cam.moving)cam.CCD.spp = 0.f;
	opt.li_sa = opt.li_sa && world.lights.size() > 0;
	opt.sun_sa = opt.sun_sa && opt.sky;
	cam.CCD.dt(opt.samples * opt.res_rate);
#if DEBUG
	invs = opt.dbg_at || opt.dbg_n || opt.dbg_uv || opt.dbg_t ? 1.f : invs;
#endif
#pragma omp parallel for schedule(dynamic,100)
	for (uint k = 0; k < cam.CCD.n; k++)
	{
		if (cam.moving) cam.CCD.clear(k);
		if (rafl() >= opt.res_rate)continue;
		uint i = k / cam.w;
		uint j = k % cam.w;
		vec3 col(0);
		float jit[2]; rafl_tuple(jit);
		float pxr = j + jit[0] - 0.5f;
		float pyr = i + jit[1] - 0.5f;
		ray r = cam.optical_ray(pyr, pxr);
#if DEBUG
		if (opt.dbg_at)		 col = raycol_at(r);
		else if (opt.dbg_n && opt.dbg_uv)  col = raycol_face(r);
		else if (opt.dbg_n)  col = raycol_n(r);
		else if (opt.dbg_uv) col = raycol_uv(r);
		else if (opt.dbg_t)  col = raycol_t(r);
		else
#endif
			col = raycol(r, invs);
		cam.pixel(i, j, col);
	}
	cam.moving = 0;
}

void scene::Optix(vector<vec3>& I, vector<vec3>& A, vector<vec3>& N)const {
	printf("Generating buffers, please wait !!!\n");
	hitrec rec; matrec mat;
	int sa = opt.en_fog ? 1000 : 10000;
	float isa = 1.f / sa;
	float threshold = opt.en_fog ? 0.01f : 100.f;
#pragma omp parallel for schedule(dynamic,100)
	for (int k = 0; k < cam.CCD.n; k++)
	{
		int i = k / cam.w;
		int j = k % cam.w;
		vec3 a, n, a100, n100;
		ray r; hitrec rec; bool hit;
		for (int f = 0; f < sa; f++) {
			float jit[2]; rafl_tuple(jit);
			float pxr = j + jit[0] - 0.5f;
			float pyr = i + jit[1] - 0.5f;
			r = cam.optical_ray(pyr, pxr);
			rec = hitrec();
			hit = world.hit(r, rec);
			vec3 rgb = hit ? world.materials[rec.mat].tex.rgb(rec.u, rec.v) : sky(r.D);
			vec3 map = hit ? world.materials[rec.mat].tex.nor(rec.u, rec.v) : 0;
			vec3 nor = hit ? normal_map(rec.N, map) : -r.D;
			if (opt.en_fog) {
				for (int s = 0; s < 100; s++)
				{
					if (opt.ninv_fog * flogf(rafl()) < rec.t) {
						a += 0.01f * isa * opt.fog_col;
					}
					else {
						a += 0.01f * isa * rgb;
						n += 0.01f * isa * nor;
					}
				}
			}
			else {
				a += isa * rgb;
				n += isa * nor;
			}
			if (f == 100 - 1) {
				a100 = a;
			}
			else if (f == 200 - 1 && near0(threshold * (2 * a100 - a))) {
				a = (sa / 200.f) * a;
				n = (sa / 200.f) * n;
				break;
			}
		}
		n = 0.5f * (n + 1.f);
		A[k] = vec3(a, 1);
		N[k] = vec3(n*n, 1);
	}
	I = cam.CCD.data;
}

void scene::Screenshot() const{
	int spp = cam.CCD.spp;
	uint wh = cam.CCD.n;
	uint w = cam.CCD.w;
	uint h = cam.CCD.h;
	char name[40] = {};
	std::tm tm; time_t now = time(0);
	localtime_s(&tm, &now);
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", &tm);
	string file;
	string name_spp = string(name) + "_" + std::to_string(spp) + "SPP_";
	vector<uint> buff(wh);
	vector<vec3> I(wh), A(wh), N(wh);
	Optix(I, A, N);

	file = "screenshots\\" + name_spp + "i.png";
	for (uint i = 0; i < wh; i++) rgb(I[i], buff[i]);
	stbi_write_png(file.c_str(), w, h, 4, &buff[0], 4 * w);
	cout << "Saved file in: " << file << "\n";

	file = "screenshots\\" + name_spp + "a.png";
	for (uint i = 0; i < wh; i++) rgb(A[i], buff[i]);
	stbi_write_png(file.c_str(), w, h, 4, &buff[0], 4 * w);
	cout << "Saved file in: " << file << "\n";

	file = "screenshots\\" + name_spp + "n.png";
	for (uint i = 0; i < wh; i++) rgb(N[i], buff[i]);
	stbi_write_png(file.c_str(), w, h, 4, &buff[0], 4 * w);
	cout << "Saved file in: " << file << "\n";

	string i = " -i screenshots\\" + name_spp + "i.png";
	string a = " -a screenshots\\" + name_spp + "a.png";
	string n = " -n screenshots\\" + name_spp + "n.png";
	string o = " -o screenshots\\" + name_spp + "o.png";
	string invoke = "denoiser\\Denoiser.exe" + i + a + n + o;
	system(invoke.c_str());
	remove(("screenshots\\" + name_spp + "a.png").c_str());
	remove(("screenshots\\" + name_spp + "n.png").c_str());
}