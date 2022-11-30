#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>
#include "scenes.h"
bool en_bvh = 1;

void scn1(scene& scn) {
	scn.world.clear();
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
	scn.opt = options();
	scn.cam.exposure = 1.f;
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(0, 1.7, 1), vec3(0, 0, 0)), 70, 1.f);
	en_bvh = 0;
}
void scn2(scene& scn) {
	scn.world.clear();
	scn.opt.li_sa = 1;
	albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 10);
	albedo white(vec3(0.8, 0.8, 0.8), vec3(0, 5, 0));
	albedo pbrcol(vec3(0.5, 0.1, 0.1), vec3(), texture("iron_block_normal.png"), 10);
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
	scn.opt = options();
	scn.world.add(room, 0);
	scn.world.add(quad(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0.999)), 1); //0.24 0 0.67
	scn.world.add(voxel(vec3(0, 1.9, 0, 0.1)), 2, 1, 1);
	scn.sun_pos.set_A(vec3(-1, 0, 0));
	scn.cam.setup(matrix(vec3(2, 1, 0), vec3(0, hpi, 0)), 90, 10);
	en_bvh = 0;
}
void scn3(scene& scn) {
	scn.world.clear();

	for (int i = 0; i <= 10; i++) {
		for (int j = 0; j <= 10; j++) {
			albedo pbrcol(vec3(0.9f, 0.6f, 0.), vec3(0.1 * i, 0, 0.1 * j), vec3(0.5, 0.5, 1), 10);
			scn.world.add_mat(pbrcol, mat_ggx);
			for (int k = 0; k <= 10; k++)
				scn.world.add(sphere(vec3(6 - j, 6 - i, -3 - k, 0.5)), i * 11 + j);
		}
	}
	scn.opt = options();
	scn.sun_pos.set_A(vec3(1, 0, 1));
	scn.cam.setup(matrix(vec3(1, 1, 10), vec3(0, 0, 0)), 47, 10);
	en_bvh = 1;
}
void scn4(scene& scn) {
	scn.world.clear();
	albedo white(vec3(0.9, 0.9, 0.9, 1), vec3(0, 0, 1));
	albedo light(vec3(0.4, 0.0, 0.7, 1), vec3(0, 100, 0));
	albedo clear(vec3(1.f, 1.f, 1.f, 0), vec3(1, 0, 0.2), vec3(0.5, 0.5, 1), 1, 1.5);
	scn.world.add_mat(white, mat_ggx);
	scn.world.add_mat(clear, mat_mix);
	scn.world.add_mat(light, mat_lig);
	scn.world.add(quad(vec3(-10, eps, -10), vec3(10, eps, -10), vec3(-10, eps, 10)), 0, 1);
	scn.world.add(voxel(vec3(0, 1 + 10 * eps, 0, 1)), 1, 1, 1);
	scn.world.add(sphere(vec3(3, 3, 3, 0.5)), 2, 1, 1);
	scn.opt = options();
	scn.opt.sky = false;
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(3, 3, -3), vec3(0, 3 * pi / 4, -pi / 6)), 60, 1.f);
	en_bvh = 0;
}
void scn5(scene& scn) {
	scn.world.clear();
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
	scn.opt = options();
	scn.opt.sky = false;
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(l / 2, 0.2f, l), vec3(0)), 40, 16.f);
	en_bvh = 0;
}
void scn6(scene& scn) {
	scn.world.clear();

	albedo gre(vec3(0.7, 0.9, 0.7, 0), vec3(0, 0, 0), vec3(0.5, 0.5, 1), 1, 1.2);
	albedo bro(vec3(0.4, 0.2, 0.0, 1), vec3(0, 0, 1));
	albedo red(vec3(0.63, 0.28, 0.25, 1), vec3(0.5, 0, 0.1));
	scn.world.add_mat(bro, mat_mix);
	scn.world.add_mat(gre, mat_mix);
	scn.world.add_mat(red, mat_ggx);

	vector<tri> gourd = load_OBJ("gourd.obj", 0, 0.5f);
	vector<tri> teapot = load_OBJ("teapot.obj", 0, 0.25f);
	//	vector<tri> dragon = load_OBJ("xyzrgb_dragon.obj", 0,0.01f);
	scn.world.add(vec3(0, 1, 0), quad(vec3(-10, 0, -10), vec3(-10, 0, 10), vec3(10, 0, -10)), 0, 1, 0);
	scn.world.add(vec3(-0.3, 1.79, -1), gourd, 1);
	scn.world.add(vec3(0.7, 1, -1), teapot, 2);
	//scn.world.add(vec3(0, 1.8, -0.5),dragon, 2);
	scn.opt = options();
	scn.cam.exposure = 1.f;
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(0.2, 1.7, 1), vec3(0, 0, 0)), 70, 64.f);
	en_bvh = 1;
}
void scn7(scene& scn) {
	scn.world.clear();

	albedo gre(vec3(0.7, 0.9, 0.7, 0), vec3(0, 0, 0), vec3(0.5, 0.5, 1), 1, 1.2);
	albedo bro(vec3(0.4, 0.2, 0.0, 1), vec3(0, 0, 1));
	albedo red(vec3(0.63, 0.28, 0, 1), vec3(0.5, 0, 0.1));
	scn.world.add_mat(red, mat_ggx);
	vector<tri> dragon = load_OBJ("xyzrgb_dragon.obj", 0, 0.01f);
	scn.world.add(vec3(0, 1.8, -0.5), dragon, 0);
	scn.opt = options();
	scn.cam.exposure = 1.f;
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(0.2, 1.7, 1), vec3(0, 0, 0)), 70, 64.f);
	en_bvh = 1;
}

std::vector<tri> load_OBJ(const char* name, vec3 off, float scale, bool flip)
{
	struct xyz {
		xyz() {}
		union {
			uint all[3] = {};
			struct {
				uint x, y, z;
			};
		};


	};
	//Vertex portions
	std::vector<vec3> v;
	std::vector<vec3> vt;
	std::vector<vec3> vn;

	//Face vectors
	std::vector<xyz> fv;
	std::vector<xyz> ft;
	std::vector<xyz> fn;

	//Vertex array
	std::vector<tri> tris;

	std::stringstream ss;
	std::ifstream file(name);
	std::string line = "";
	std::string pref = "";

	if (!file.is_open())
	{
		file = std::ifstream("objects/" + std::string(name));
		if (!file.is_open())
			throw "File not found !";
	}
	vector<char> buffer;
	buffer.resize(1024 * 1024, 0);
	file.rdbuf()->pubsetbuf(&buffer[0], buffer.size());
	int size = file.tellg();;
	v.reserve(size / 2);
	vt.reserve(size / 2);
	vn.reserve(size / 2);
	fv.reserve(size / 2);
	ft.reserve(size / 2);
	fn.reserve(size / 2);
	while (std::getline(file, line))
	{
		ss.clear();
		ss.str(line);
		ss >> pref;
		if (pref == "#");
		else if (pref == "o");
		else if (pref == "s");
		else if (pref == "use_mtl");
		else if (pref == "v")
		{
			vec3 tmp;
			ss >> tmp._xyz[0] >> tmp._xyz[1] >> tmp._xyz[2];
			v.emplace_back(tmp);
		}
		else if (pref == "vt")
		{
			vec3 tmp;
			ss >> tmp._xyz[0] >> tmp._xyz[1];
			vt.emplace_back(tmp);
		}
		else if (pref == "vn")
		{
			vec3 tmp;
			ss >> tmp._xyz[0] >> tmp._xyz[1] >> tmp._xyz[2];
			vn.emplace_back(tmp);
		}
		else if (pref == "f")
		{
			xyz buff = {};
			ss >> buff.x >> buff.y >> buff.z;
			buff.x -= 1;
			buff.y -= 1;
			buff.z -= 1;
			fv.emplace_back(buff);
		}
		else;
	}
	tris.resize(fv.size());
	for (size_t i = 0; i < fv.size(); i++)
	{
		vec3 a = scale * v[fv[i].all[0]];
		vec3 b = scale * v[fv[i].all[1]];
		vec3 c = scale * v[fv[i].all[2]];
		tris[i] = flip ? tri(a, c, b) : tri(a, b, c);
		//if (vn.size() > 0) tris[i] = tri(a, b, c, vn[fv[i].x()], vn[fv[i].y()], vn[fv[i].z()]);
	}
	std::cout << "Succesfully loaded: " << name << "\n";
	std::cout << "No of tris: " << tris.size() << "\n";
	//Loaded success

	return tris;
}