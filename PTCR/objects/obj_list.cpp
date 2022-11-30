#include "obj_list.h"
void obj_list::clear()
{
	bvh.clear();
	obj_bvh.clear();
	skp_bvh.clear();
	objects.clear();
	lights.clear();
	materials.clear();
	bbox = aabb();
}
obj_id obj_list::get_id(const ray& r, hitrec& rec) const
{
	if (!bbox.hit(r)) return obj_id();
	obj_id res;
	for (uint i = 0; i < objects.size(); i++)
		if (objects[i].hit(r, rec))res = obj_id(objects[i].id, i, 1);
	for (uint i = 0; i < skp_bvh.size(); i++)
		if (skp_bvh[i].hit(r, rec))res = obj_id(skp_bvh[i].id, i, 0);
	return res;
}
void obj_list::get_trans(const obj_id id, matrix& T)const
{
	if (id.bvh) T = objects[id.id].get_trans();
	else T = skp_bvh[id.id].get_trans();

}
void obj_list::set_trans(obj_id id, const matrix& T, uint node_size) {
	if (id.bvh) {
		objects[id.id].set_trans(T);
		rebuild_bvh(0, node_size);
	}
	else {
		skp_bvh[id.id].set_trans(T);
	}
	fit();
}
void obj_list::fit() {
	bbox = aabb();
	for (const auto& obj : objects)
		bbox.join(obj.get_box());
	for (const auto& obj : skp_bvh)
		bbox.join(obj.get_box());
}
void obj_list::obj_unroll() {
	size_t size = 0;
	for (const auto& obj : objects)
		size += obj.get_size();
	obj_bvh.reserve(size);
	for (const auto& obj : objects)
	{
		uint mat = obj.get_mat();
		for (uint j = 0; j < obj.get_size(); j++)
		{
#if BVHSMALL
			obj_bvh.emplace_back(mesh_raw(&obj, j));
#else
			switch (obj.id) {
			case o_tri: obj_bvh.emplace_back(mesh_raw(obj.t.get_data(j), mat)); break;
			case o_qua: obj_bvh.emplace_back(mesh_raw(obj.q.get_data(j), mat)); break;
			case o_sph: obj_bvh.emplace_back(mesh_raw(obj.s.get_data(j), mat)); break;
			case o_vox: obj_bvh.emplace_back(mesh_raw(obj.v.get_data(j), mat)); break;
			default:break;
			}
#endif
		}
	}
}
void obj_list::rebuild_bvh(bool print, uint node_size) {
	bvh.clear();
#pragma omp parallel for schedule(static,64)
	for (auto& obj : obj_bvh)
		obj.update_box();
	split_bvh(0, obj_bvh.size(), node_size);
}

void obj_list::build_bvh(bool print, uint node_size) {
	time_t t1 = clock();
	obj_unroll();
	bvh.reserve(obj_bvh.size());
	split_bvh(0, obj_bvh.size(), node_size);
	bvh.shrink_to_fit();
	float t2 = clock() - t1;
	if (print)
		printf("\n%d %f\n", (int)bvh.size(), t2 / CLOCKS_PER_SEC);
}
//hard to believe but this creates the best memory layout (left node is next to current, right node has large offset)
void obj_list::split_bvh(uint be, uint en, uint node_size) {
	aabb bbox = box_from(be, en);
	uint size = en - be;
	if (size > node_size) {
		uint half = size / 2;
		uint n = bvh.size();
		uchar axis = bbox.get_longest_axis();
		auto begin = obj_bvh.begin() + be;
		auto mid = begin + half;
		auto end = begin + size;
		switch (axis) {
		case 0: std::nth_element(begin, mid, end, cmp_axis_x); break;
		case 1: std::nth_element(begin, mid, end, cmp_axis_y); break;
		default: std::nth_element(begin, mid, end, cmp_axis_z); break;
		}
		bvh.emplace_back(bvh_node(bbox, n + 1, 0, 1));
		split_bvh(be, be + half, node_size);
		bvh[n].n2 = bvh.size();
		split_bvh(be + half, be + size, node_size);
	}
	else bvh.emplace_back(bvh_node(bbox, be, en, 0));
	
}

aabb obj_list::box_from(uint beg, uint end) {
	vec3 pmin = obj_bvh[beg].get_box().pmin;
	vec3 pmax = obj_bvh[beg].get_box().pmax;
	for (uint i = beg + 1; i < end; i++)
	{
		pmin = min(obj_bvh[i].get_box().pmin, pmin);
		pmax = max(obj_bvh[i].get_box().pmax, pmax);
	}
	return aabb(pmin,pmax);
}