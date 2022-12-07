#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>
#include "scenes.h"

void scn1(scene& scn) {
	albedo gre(vec3(0.7, 0.9, 0.7, 0), vec3(0, 0, 0), vec3(0.5, 0.5, 1), 1, 1.2);
	albedo dgre(vec3(0.4, 0.51, 0.0, 1), vec3(0, 0, 1), "snow_normal.png", 10);
	albedo blu(vec3(0.1, 0.28, 0.8, 1), vec3(0.5, 0, 0.1));
	scn.world.add_mat(dgre, mat_mix);
	scn.world.add_mat(gre, mat_mix);
	scn.world.add_mat(blu, mat_ggx);
	vector<poly> bunny = load_OBJ("bunny.obj", 0, 6.f);
	vector<poly> teapot = load_OBJ("teapot.obj", 0, 0.25f);
	scn.world.add(vec3(0, 1, 0), quad(vec3(-10, 0, -10), vec3(-10, 0, 10), vec3(10, 0, -10)), 0, 1, 0);
	scn.world.add(vec3(-0.44, 0.8, -1), bunny, 1);
	scn.world.add(vec3(0.7, 1, -1), teapot, 2);
	scn.cam.autofocus = 0;
	scn.cam.foc_t = 1.556;
	scn.cam.exposure = 1.f;
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.opt.bounces = 10;
	scn.opt.ninv_fog = -5;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(0.1, 1.4, 0.8), vec3(0, 0, 0)), 70, 1.f);
	scn.world.en_bvh = 1;
}

void scn2(scene& scn) {
	scn.opt.li_sa = 1;
	albedo iron(vec3(0.7, 0.7, 0.9), vec3(1, 0, 0));
	albedo white(vec3(0.8, 0.8, 0.8), vec3(0, 5, 0));
	scn.world.add_mat(albedo(vec3(0.8, 0.8, 0.8, 1), vec3(0, 0, 1)), mat_mix);
	scn.world.add_mat(iron, mat_ggx);
	scn.world.add_mat(white, mat_lig);
	vector<quad> room(6);
	room.emplace_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 0, 1)));
	room.emplace_back(quad(vec3(-1, 0, -1), vec3(-1, 2, -1), vec3(-1, 0, 1)));
	room.emplace_back(quad(vec3(3, 0, -1), vec3(3, 1.5, -1), vec3(3, 0, 1)));
	room.emplace_back(quad(vec3(-1, 2, -1), vec3(3, 2, -1), vec3(-1, 2, 1)));
	room.emplace_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 2, -1)));
	room.emplace_back(quad(vec3(-1, 0, 1), vec3(3, 0, 1), vec3(-1, 2, 1)));
	scn.opt.ninv_fog = -100;
	scn.world.add(room, 0);
	scn.world.add(quad(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0.999)), 1); //0.24 0 0.67
	scn.world.add(voxel(vec3(0, 1.9, 0, 0.1)), 2, 1, 1);
	scn.sun_pos.set_A(vec3(-1, 0, 0));
	scn.cam.setup(matrix(vec3(2, 1, 0), vec3(0, hpi, 0)), 90, 10);
	scn.world.en_bvh = 0;
}
void scn3(scene& scn) {
	for (int i = 0; i <= 10; i++) {
		for (int j = 0; j <= 10; j++) {
			albedo pbrcol(vec3(0.8f, 0.1f, 0.1f), vec3(0.1 * i, 0, 0.1 * j), vec3(0.5, 0.5, 1), 10);
			scn.world.add_mat(pbrcol, mat_ggx);
			scn.world.add(vec3(6 - i, 6 - j, -3), sphere(vec3(0, 0, 0, 0.5)), i * 11 + j);
		}
	}
	scn.opt.en_fog = 0;
	scn.sun_pos.set_A(vec3(1, 0, 1));
	scn.cam.setup(matrix(vec3(1, 1, 10), vec3(0, 0, 0)), 47, 10);
	scn.world.en_bvh = 1;
}
void scn4(scene& scn) {
	albedo white(vec3(0.9, 0.9, 0.9, 1), vec3(0, 0, 1));
	albedo light(vec3(0.4, 0.0, 0.7, 1), vec3(0, 100, 0));
	albedo clear(vec3(1.f, 1.f, 1.f, 0), vec3(1, 0, 0.2), vec3(0.5, 0.5, 1), 1, 1.5);
	scn.world.add_mat(white, mat_ggx);
	scn.world.add_mat(clear, mat_mix);
	scn.world.add_mat(light, mat_lig);
	scn.world.add(quad(vec3(-10, eps, -10), vec3(10, eps, -10), vec3(-10, eps, 10)), 0, 1);
	scn.world.add(voxel(vec3(0, 1 + 10 * eps, 0, 1)), 1, 1, 1);
	scn.world.add(sphere(vec3(3, 3, 3, 0.5)), 2, 1, 1);
	scn.opt.sky = false;
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(3, 3, -3), vec3(0, 3 * pi / 4, -pi / 6)), 60, 1.f);
	scn.world.en_bvh = 0;
}
void scn5(scene& scn) {

	albedo white(vec3(0.73, 0.73, 0.73, 1), vec3(0, 0, 1));
	albedo green(vec3(0.12, 0.45, 0.12, 1), vec3(0, 0, 1));
	albedo red(vec3(0.45, 0.12, 0.12, 1), vec3(0, 0, 1));
	albedo blue(vec3(0.12, 0.12, 0.45, 1), vec3(0, 0, 1));
	albedo light(vec3(1), vec3(0, 100, 0));
	albedo clear(vec3(0.73, 0.73, 0.73, 0), vec3(1, 0, 0.1), vec3(0.5, 0.5, 1), 1, 1.5);
	albedo mirror(vec3(0.73, 0.73, 0.73, 1.f), vec3(1, 0, 0.1));
	scn.world.add_mat(white, mat_mix);
	scn.world.add_mat(red, mat_mix);
	scn.world.add_mat(green, mat_mix);
	scn.world.add_mat(blue, mat_mix);
	scn.world.add_mat(clear, mat_mix);
	scn.world.add_mat(mirror, mat_ggx);
	scn.world.add_mat(light, mat_lig);
	constexpr float l = 0.555f;
	scn.world.add(quad(vec3(0, 0, 0), vec3(l, 0, 0), vec3(0, 0, -l)), 0);
	scn.world.add(quad(vec3(0, l, 0), vec3(l, l, 0), vec3(0, l, -l)), 0);
	scn.world.add(quad(vec3(0, 0, -l), vec3(l, 0, -l), vec3(0, l, -l)), 3);
	scn.world.add(quad(vec3(0, 0, 0), vec3(0, 0, -l), vec3(0, l, 0)), 1);
	scn.world.add(quad(vec3(l, 0, 0), vec3(l, 0, -l), vec3(l, l, 0)), 2);
	scn.world.add(quad(vec3(0.213, l - eps, -0.227), vec3(0.343, l - eps, -0.227), vec3(0.213, l - eps, -0.332)), 6, 1, 1);
	scn.world.add(sphere(vec3(l / 4, 0.1, -l / 4, 0.1)), 0);
	scn.world.add(sphere(vec3(l - l / 4, 0.1, -l / 4, 0.1)), 4);
	scn.world.add(sphere(vec3(l / 2, 0.1, -l + l / 4, 0.1)), 5);
	scn.opt.ninv_fog = -2;
	scn.opt.sky = false;
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(l / 2, 0.2f, l), vec3(0)), 40, 16.f);
	scn.world.en_bvh = 0;
}

void scn6(scene& scn) {
	albedo gnd(vec3(0.8, 0.4, 0.3, 1), vec3(0, 0, 0));
	albedo trans(vec3(0.7, 0.7, 0.99, 0), vec3(1, 0, 0.2), vec3(0.5, 0.5, 1), 1, 1.2);
	albedo trans2(vec3(0.99, 0.7, 0.7, 0), vec3(1, 0, 0.2), vec3(0.5, 0.5, 1), 1, 1.2);
	albedo red(vec3(0.8, 0.1, 0.1, 1), vec3(1, 0, 0.1));
	albedo blue(vec3(0.1, 0.1, 0.8, 1), vec3(1, 0, 0.1));
	scn.world.add_mat(gnd, mat_ggx);
	scn.world.add_mat(trans, mat_mix);
	scn.world.add_mat(red, mat_ggx);
	scn.world.add_mat(trans2, mat_mix);
	scn.world.add_mat(blue, mat_ggx);
	scn.world.add(quad(vec3(-100, 0, -100), vec3(100, 0, -100), vec3(-100, 0, 100)), 0, 1);
	scn.world.add(sphere(vec3(0, 3.001, -3, 1)), 1);
	scn.world.add(voxel(vec3(0, 3.001, -3, 0.576)), 2);
	scn.world.add(voxel(vec3(0, 1.001, -3, 1)), 3);
	scn.world.add(sphere(vec3(0, 1.001, -3, 0.999f)), 4);
	scn.opt.ninv_fog = -100;
	scn.cam.exposure = 1.f;
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(0, 1.7, 1), vec3(0, 0, 0)), 70, 1.f);
	scn.world.en_bvh = 0;
}

void scn7(scene& scn) {
	albedo red(vec3(0.63, 0.28, 0, 1), vec3(0.5, 20, 0.1));
	scn.world.add_mat(red, mat_las);
	vector<poly> dragon = load_OBJ("xyzrgb_dragon.obj", 0, 0.01f);
	scn.world.add(matrix(vec3(0, 1.8, -0.5), vec3(0, -0.66, 0)), dragon, 0);
	scn.cam.exposure = 1.f;
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.opt.ninv_fog = -0.5;
	scn.opt.en_fog = true;
	scn.cam.setup(matrix(vec3(0.2, 1.7, 1), vec3(0, 0, 0)), 70, 64.f);
	scn.world.en_bvh = 1;
}

void scn8(scene& scn) {
	albedo r(vec3(0.8, 0.1, 0.1, 1), vec3(0, 1000, 0));
	albedo g(vec3(0.1, 0.8, 0.1, 1), vec3(0, 1000, 0));
	albedo b(vec3(0.1, 0.1, 0.8, 1), vec3(0, 1000, 0));
	albedo a(vec3(0.95, 0.95, 0.95, 0), vec3(0, 0, 0), vec3(0.5, 0.5, 1), 1, 1.5);
	scn.world.add_mat(a, mat_mix);
	scn.world.add_mat(r, mat_las);
	scn.world.add_mat(g, mat_las);
	scn.world.add_mat(b, mat_las);
	scn.world.add(vec3(0, 0, -0.1), sphere(vec3(0, 0, 0, 0.03)), 0, 1, 1);
	scn.world.add(vec3(-0.05, 0.09, -0.1), sphere(vec3(0, 0, 0, 0.01)), 1, 1, 1);
	scn.world.add(vec3(0, 0.1, -0.1), sphere(vec3(0, 0, 0, 0.01)), 2, 1, 1);
	scn.world.add(vec3(0.05, 0.09, -0.1), sphere(vec3(0, 0, 0, 0.01)), 3, 1, 1);
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 20;
	scn.opt.samples = 2;
	scn.opt.p_life = 1.f;
	scn.opt.i_life = 1.f;
	scn.opt.ninv_fog = -1;
	scn.opt.en_fog = true;
	scn.opt.sky = 0;
	scn.cam.setup(matrix(vec3(0, 0, 0), vec3(0, 0, 0)), 70, 64.f);
	scn.world.en_bvh = 0;
}


std::vector<poly> load_OBJ(const char* name, vec3 off, float scale, bool flip)
{
	struct uint3 {
		uint x, y, z;
	};
	std::ifstream file(name);
	if (!file.is_open())
	{
		file = std::ifstream("objects/" + std::string(name));
		if (!file.is_open())
			throw "File not found !";
	}
	std::stringstream file_buff;
	file_buff << file.rdbuf();
	std::vector<vec3> vert; vert.reserve(0xfffff);
	std::vector<uint3> face; face.reserve(0xfffff);
	std::vector<poly> tris; tris.reserve(0xfffff);
	std::string line = "";
	std::string pref = "";
	float t1 = clock();
	while (std::getline(file_buff, line))
	{
//C version is 2x faster
#if 1
		if (line[0] == 'v'){
			char b;
			vec3 tmp;
			sscanf(line.c_str(), "%c %f %f %f", &b, &tmp._xyz[0], &tmp._xyz[1], &tmp._xyz[2]);
			vert.emplace_back(tmp);
		}
		else if (line[0] == 'f') {
			char b;
			uint3 tmp;
			sscanf(line.c_str(), "%c %u %u %u", &b, &tmp.x, &tmp.y, &tmp.z);
			tmp.x -= 1; tmp.y -= 1; tmp.z -= 1;
			face.emplace_back(tmp);
		}
#else
		std::stringstream ss;
		ss.str(line);
		ss >> pref;
		if (pref == "v")
		{
			vec3 tmp;
			ss >> tmp._xyz[0] >> tmp._xyz[1] >> tmp._xyz[2];
			vert.emplace_back(tmp);
		}
		else if (pref == "f")
		{
			uint3 tmp = {};
			ss >> tmp.x >> tmp.y >> tmp.z;
			tmp.x -= 1; tmp.y -= 1; tmp.z -= 1;
			face.emplace_back(tmp);
		}
#endif
	}
	for (auto& v : vert)
		v *= scale;
	for (const auto& f : face)
	{
		vec3 a = vert[f.x];
		vec3 b = vert[f.y];
		vec3 c = vert[f.z];
		tris.emplace_back(flip ? poly(a, c, b) : poly(a, b, c));
	}
	std::cout << "Loaded: " << name << "\n";
	std::cout << "No of tris: " << tris.size() << " Took: " << (clock() - t1) / CLOCKS_PER_SEC << "\n";
	return tris;
}