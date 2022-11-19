#include "scenes.h"
void scn1(scene& scn) {
	scn.world.clear();
	/*albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 2e2);
	albedo snow = albedo(texture("snow.png"), texture("snow_mer.png"), texture("snow_normal.png"), 2e3);*/
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
	scn.world.add(quad(vec3(-100, 0, -100), vec3(100, 0, -100), vec3(-100, 0, 100)), 0);
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
}
void scn2(scene& scn) {
	scn.world.clear();
	scn.opt.li_sa = 1;
	albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 10);
	albedo white(vec3(0.8, 0.8, 0.8), vec3(0, 5, 0));
	albedo pbrcol(vec3(0.5, 0.1, 0.1), vec3(), texture("iron_block_normal.png"), 10);
	scn.world.add_mat(albedo(vec3(0.8, 0.8, 0.8,1), vec3(0,0,1)), mat_mix);
	scn.world.add_mat(iron, mat_ggx);
	scn.world.add_mat(white, mat_lig);
	vector<quad> room(6);
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(-1, 2, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(3, 0, -1), vec3(3, 1.5, -1), vec3(3, 0, 1)));
	room.push_back(quad(vec3(-1, 2, -1), vec3(3, 2, -1), vec3(-1, 2, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 2, -1)));
	room.push_back(quad(vec3(-1, 0, 1), vec3(3, 0, 1), vec3(-1, 2, 1)));
	scn.opt = options();
	scn.world.add(room, 0);
	scn.world.add(quad(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0.999)), 1); //0.24 0 0.67
	scn.world.add(voxel(vec3(0, 1.9, 0, 0.1)), 2, 1);
	scn.sun_pos.set_A(vec3(-1, 0, 0));
	scn.cam.setup(matrix(vec3(2, 1, 0), vec3(0, hpi, 0)), 90, 10);
}
void scn3(scene& scn) {
	scn.world.clear();
	for (int i = 0; i <= 10; i++) {
		for (int j = 0; j <= 10; j++) {
			albedo pbrcol(vec3(0.9f, 0.6f, 0.), vec3(0.1 * i, 0, 0.1 * j), vec3(0.5, 0.5, 1), 10);
			scn.world.add_mat(pbrcol, mat_vnd);
			scn.world.add(sphere(vec3(6 - j, 6 - i, -3, 0.5)), i * 11 + j);
		}
	}
	scn.opt = options();
	scn.sun_pos.set_A(vec3(1, 0, 1));
	scn.cam.setup(matrix(vec3(1, 1, 10), vec3(0, 0, 0)), 47, 10);
}
void scn4(scene& scn) {
	scn.world.clear();
	albedo white(vec3(0.9, 0.9, 0.9, 1), vec3(0, 0, 1));
	albedo light(vec3(0.4, 0.0, 0.7, 1), vec3(0, 100, 0));
	albedo clear(vec3(1.f, 1.f, 1.f, 0), vec3(1, 0, 0.2),vec3(0.5,0.5,1), 1, 1.5);
	scn.world.add_mat(white, mat_ggx);
	scn.world.add_mat(clear, mat_mix);
	scn.world.add_mat(light, mat_lig);
	scn.world.add(quad(vec3(-10, eps, -10), vec3(10, eps, -10), vec3(-10, eps, 10)), 0);
	scn.world.add(voxel(vec3(0, 1+10*eps, 0, 1)), 1, 1);
	scn.world.add(sphere(vec3(3, 3, 3, 0.5)), 2 , 1);
	scn.opt = options();
	scn.opt.sky = false;
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(3, 3, -3), vec3(0, 3*pi/4, -pi/6)), 60, 1.f);
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
	scn.world.add(quad(vec3(0, 0, 0), vec3(l, 0, 0), vec3(0, 0, -l)),0);
	scn.world.add(quad(vec3(0, l, 0), vec3(l, l, 0), vec3(0, l, -l)),0);
	scn.world.add(quad(vec3(0, 0, -l), vec3(l, 0, -l), vec3(0, l, -l)),3);
	scn.world.add(quad(vec3(0, 0, 0), vec3(0, 0, -l), vec3(0, l, 0)), 1);
	scn.world.add(quad(vec3(l, 0, 0), vec3(l, 0, -l), vec3(l, l, 0)), 2);
	scn.world.add(quad(vec3(0.213, l - eps, -0.227), vec3(0.343, l - eps, -0.227), vec3(0.213, l - eps, -0.332)), 6, 1);
	scn.world.add(sphere(vec3(l / 4, 0.1, -l / 4, 0.1)), 0);
	scn.world.add(sphere(vec3(l-l / 4, 0.1, -l / 4, 0.1)), 4);
	scn.world.add(sphere(vec3(l/2, 0.1, -l+l / 4, 0.1)), 5);
	scn.opt = options();
	scn.opt.sky = false;
	scn.cam.exposure = 1.f;
	scn.opt.bounces = 10;
	scn.opt.p_life = 0.9f;
	scn.opt.i_life = 1.f / 0.9f;
	scn.cam.setup(matrix(vec3(l/2, 0.2f, l),vec3(0)), 40, 16.f);
}