#pragma once
#include "scene.h"
constexpr int no_scn = 5;

void scn1(scene& scn);
void scn2(scene& scn);
void scn3(scene& scn);
void scn4(scene& scn);
void scn5(scene& scn);

inline void scn_load(scene& scn, int n) {
	switch (n) {
	case 1: scn1(scn); break;
	case 2: scn2(scn); break;
	case 3: scn3(scn); break;
	case 4: scn4(scn); break;
	case 5: scn5(scn); break;
	default: scn1(scn); break;
	}
	scn.world.build_bvh();
}
