#include "scenes.h"
void scn1(scene& scn) {
	scn.world.clear();
	albedo iron = albedo(texture("iron_block.png"),0, texture("iron_block_normal.png"), 2e2);
	albedo snow = albedo(texture("snow.png"), texture("snow_mer.png"), texture("snow_normal.png"), 2e3);
	albedo pbrcol(vec3(0.8, 0.4, 0.0), vec3(1, 0, 0.1), texture("gravel_normal.png"), 10);
	albedo pbrcol2(vec3(0.0, 0.8, 0.8), vec3(1, 0, 0.1), texture("gravel_normal.png"), 10);
	albedo white(vec3(0.8, 0.8, 0.8),vec3(0,10,0));
	scn.world.add_mat(snow, mat_ggx);
	scn.world.add_mat(pbrcol, mat_ggx);
	scn.world.add_mat(pbrcol2, mat_ggx);
	scn.world.add(quad(vec3(-100, eps, -100), vec3(100, eps, -100), vec3(-100, eps, 100)), 0);
	scn.world.add(sphere(vec3(-1, 0.95f, -3, 1)), 1);
	scn.world.add(sphere(vec3(1, 0.95f, -3, 1)), 2);
	scn.sun_pos.set_A(vec3(1, 0, 0.32));
	scn.cam.setup(matrix(vec3(0, 1, 0), vec3(0, 0, 0)), 70, 1);
}
void scn2(scene& scn) {
	scn.world.clear();
	scn.opt.li_sa = 1;
	albedo iron = albedo(texture("iron_block.png"), texture("iron_block_mer.png"), texture("iron_block_normal.png"), 10);
	albedo white(vec3(0.8, 0.8, 0.8), vec3(0, 5, 0));
	albedo pbrcol(vec3(0.5, 0.1, 0.1), vec3(), texture("iron_block_normal.png"), 10);
	scn.world.add_mat(albedo(vec3(0.8, 0.8, 0.8), 0), mat_mix);
	scn.world.add_mat(iron, mat_ggx);
	scn.world.add_mat(white, mat_uli);
	vector<quad> room(6);
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(-1, 2, -1), vec3(-1, 0, 1)));
	room.push_back(quad(vec3(3, 0, -1), vec3(3, 1.5, -1), vec3(3, 0, 1)));
	room.push_back(quad(vec3(-1, 2, -1), vec3(3, 2, -1), vec3(-1, 2, 1)));
	room.push_back(quad(vec3(-1, 0, -1), vec3(3, 0, -1), vec3(-1, 2, -1)));
	room.push_back(quad(vec3(-1, 0, 1), vec3(3, 0, 1), vec3(-1, 2, 1)));
	scn.world.add(room, 0);
	scn.world.add(quad(vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0.999)), 1); //0.24 0 0.67
	scn.world.add(voxel(vec3(0, 1.9, 0, 0.1)), 2, 1);
	scn.sun_pos.set_A(vec3(-1, 0, 0));
	scn.cam.setup(matrix(vec3(2, 1, 0), vec3(0, hpi, 0)), 90);
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
	scn.sun_pos.set_A(vec3(1, 0, 1));
	scn.cam.setup(matrix(vec3(1, 1, 10), vec3(0, 0, 0)), 47);
}