#include <omp.h>
#include <ctime>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
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
		world.set_trans(id, T,node_size);
	}
}

void scene::Render(uint *disp, uint pitch) {
	//write straight to texture (tiny bit less overhead)
	cam.CCD.set_disp(disp, pitch);
	float invs = 1.f / fmaxf(opt.samples, 1);
	cam_autofocus();
	if (cam.moving)cam.CCD.spp = 0.f;
	opt.li_sa = opt.li_sa && world.lights.size() > 0;
	opt.sun_sa = opt.sun_sa && opt.sky;
	cam.CCD.dt(opt.samples*opt.res_rate);
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
		else if (opt.dbg_n&& opt.dbg_uv)  col = raycol_face(r);
		else if (opt.dbg_n)  col = raycol_n(r);
		else if (opt.dbg_uv) col = raycol_uv(r);
		else if (opt.dbg_t)  col = raycol_t(r);
		else
#endif
			col = opt.en_fog ? raycol_fog(r,invs) : raycol(r,invs);
		cam.pixel(i, j, col);
	}
	cam.moving = 0;
}

void median_filter(vector<vec3>& in, vector<vec3>& out, int h, int w)
{
#pragma omp parallel for collapse(2) schedule(dynamic,100)
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			vec3 window[9];
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++)
				{
					int a = i + k - 1;
					int b = j + l - 1;
					a = (a < 0 || a >= h) ? i : a;
					b = (b < 0 || b >= w) ? j : b;
					window[k * 3 + l] = in[a * w + b];
				}
			out[i * w + j] = med9(window);
		}
	}
}

void scene::out_optix(vector<vec3>& I, vector<vec3>& A, vector<vec3>& N)const {
	hitrec rec; matrec mat;
	int sa = 2000;
	float isa = 1.f / sa;
	//vector<vec3> tA(cam.CCD.n), tN(cam.CCD.n);
#pragma omp parallel for schedule(dynamic,100)
	for (int k = 0; k < cam.CCD.n; k++)
	{
		int i = k / cam.w;
		int j = k % cam.w;
		vec3 a(0), n(0);
		hitrec rec; matrec mat; 
		ray r;
		bool hit = 0;
		for (int s = 0; s < sa; s++) {
			if (s % 10 == 0) {
				float jit[2]; rafl_tuple(jit);
				float pxr = j + jit[0] - 0.5f;
				float pyr = i + jit[1] - 0.5f;
				rec = hitrec();
				r = ray(cam.raycast(pyr, pxr));
				hit = world.hit(r, rec);
			}
			float ft = opt.ninv_fog * logf(rafl());
			if (opt.en_fog && ft < rec.t) {
				a += isa;
				n += isa * 0.5f * (sa_sph() + 1.f);
			}
			else if (hit) {
				a += isa * world.materials[rec.mat].tex.rgb(rec.u, rec.v);
				vec3 map = world.materials[rec.mat].tex.nor(rec.u, rec.v);
				n += isa * 0.5f * (normal_map(rec.N, map) + 1.f);
			}
			else {
				a += isa * sky(r.D);
				n += isa * 0.5f*(-r.D+1.f);	
			}
		}
		A[k] = vec3(a,1);
		N[k] = vec3(n, 1);
	}
	//median_filter(tA, A, cam.CCD.h, cam.CCD.w);
	//median_filter(tN, N, cam.CCD.h, cam.CCD.w);
	I = cam.CCD.data;
}



void save_hdr(const scene& scn) {
	int spp = scn.cam.CCD.spp;
	uint wh = scn.cam.CCD.n;
	uint w = scn.cam.CCD.w;
	uint h = scn.cam.CCD.h;
	static char name[20];
	time_t now = time(0);
	std::tm tm;
	localtime_s(&tm, &now);
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", &tm);
	std::string file;
	std::string name_spp = std::string(name) + "_" + std::to_string(spp) + "SPP_";
	vector<uint> buff(wh);
	vector<vec3> I(wh), A(wh), N(wh);
	scn.out_optix(I, A, N);
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

	std::string i = " -i screenshots\\" + name_spp + "i.png";
	std::string a = " -a screenshots\\" + name_spp + "a.png";
	std::string n = " -n screenshots\\" + name_spp + "n.png";
	std::string o = " -o screenshots\\" + name_spp + "o.png";
	std::string invoke = "denoiser\\Denoiser.exe" + i + a + n + o ;
	system(invoke.c_str());
	remove(("screenshots\\" + name_spp + "a.png").c_str());
	remove(("screenshots\\" + name_spp + "n.png").c_str());

}