#pragma once
#include "ray.h"
#include "matrix.h"
#include "aabb.h"
#include "onb.h"
extern bool en_bvh;
enum obj_enum {
  o_pol, o_qua, o_sph, o_vox, o_bla
};
struct obj_id {
	obj_id(obj_enum type = o_bla, uint id = 0, bool bvh = 1) :type(type), id(id), bvh(bvh) {}
	obj_enum type;
	uint id;
	bool bvh;
	inline bool operator ==(obj_id o)const {
		return type == o.type && id == o.id;
	}
};
// 32 + 8 + 1 bytes
struct bvh_node {
	// 1 = parent, 0 = leaf
	bvh_node(){}
	bvh_node(aabb bbox, uint n1, uint n2, bool flag = 0) :bbox(bbox), n1(n1), n2(n2), flag(flag) {}
	aabb bbox;
	uint n1, n2;
	bool flag;
};

struct hitrec
{
	vec3 N, P;
	float t = infp;
	float u, v;
	uint mat;
	bool face;
};


#include "material.h"

