#pragma once
#include "ray.h"
#include "matrix.h"
#include "aabb.h"
#include "onb.h"

enum obj_enum{
	o_bla, o_sph, o_qua, o_tri, o_vox, o_bvh
};
struct obj_id {
	obj_id(obj_enum type = o_bla, uint id = 0) :type(type), id(id) {}
	obj_enum type;
	uint id;
	inline bool operator ==(obj_id o)const{
		return type == o.type && id == o.id;
	}
};

struct bvh_node {
	aabb bbox;
	uint left, right;
	bool leaf;
};



struct hitrec
{
	vec3 N, P;
	uint mat;
	float t=infp;
	float u, v;	
	bool face;
};






#include "material.h"

