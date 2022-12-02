#pragma once
#include "scene.h"
constexpr int no_scn = 8;

void scn1(scene& scn);
void scn2(scene& scn);
void scn3(scene& scn);
void scn4(scene& scn);
void scn5(scene& scn);
void scn6(scene& scn);
void scn7(scene& scn);
void scn8(scene& scn);
std::vector<tri> load_OBJ(const char* name, vec3 off = 0, float scale = 1.f, bool flip_face = 0);

inline void scn_load(scene& scn, int n, uint node_size = 8) {
	switch (n) {
	case 1: scn1(scn); break;
	case 2: scn2(scn); break;
	case 3: scn3(scn); break;
	case 4: scn4(scn); break;
	case 5: scn5(scn); break;
	case 6: scn6(scn); break;
	case 7: scn7(scn); break;
	case 8: scn8(scn); break;
	default: scn1(scn); break;
	}
	scn.world.build_bvh(1, node_size);
}
