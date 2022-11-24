#pragma once
#include "ray.h"
#include "matrix.h"
#include "aabb.h"
#include "onb.h"
#define node4 0
enum obj_enum {
	o_bla, o_sph, o_qua, o_tri, o_vox, o_bvh
};
struct obj_id {
	obj_id(obj_enum type = o_bla, uint id = 0) :type(type), id(id) {}
	obj_enum type;
	uint id;
	inline bool operator ==(obj_id o)const {
		return type == o.type && id == o.id;
	}
};
//WIP
#if node4
struct bvh_node {
	// 0 = empty, 1 = one object (left node),2 = two objects,  3 = parent/recursive node
	bvh_node(aabb bbox, uint n1, uint n2, uint n3, uint n4, uchar flag = 0) :bbox(bbox), n1(n1), n2(n2), n3(n3), n4(n4), flag(flag) {}
	aabb bbox;
	uint n1, n2, n3, n4;
	uchar flag;
};
#else
struct bvh_node {
bvh_node(aabb bbox, uint n1, uint n2, uchar flag = 0) :bbox(bbox), n1(n1), n2(n2), flag(flag) {}
aabb bbox;
uint n1, n2;
uchar flag;
};
#endif
struct hitrec
{
	vec3 N, P;
	uint mat;
	float t = infp;
	float u, v;
	bool face;
};


#include "material.h"

