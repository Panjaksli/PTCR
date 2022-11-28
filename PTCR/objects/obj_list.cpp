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

void obj_list::rebuild_bvh(bool print, uint node_size) {
	bvh.clear();
	obj_bvh.clear();
	build_bvh(print, node_size);
}
void obj_list::obj_unroll() {
	size_t size = 0;
	for (const auto& obj : objects)
		size += obj.get_size();
	obj_bvh.reserve(size);
	for (auto& obj : objects)
	{
		uint mat = obj.get_mat();
		for (uint i = 0; i < obj.get_size(); i++)
			switch (obj.id) {
			case o_tri: obj_bvh.push_back(mesh_raw(obj.t.get_data(i), mat)); break;
			case o_qua: obj_bvh.push_back(mesh_raw(obj.q.get_data(i), mat)); break;
			case o_sph: obj_bvh.push_back(mesh_raw(obj.s.get_data(i), mat)); break;
			case o_vox: obj_bvh.push_back(mesh_raw(obj.v.get_data(i), mat)); break;
			default:break;
			}

	}
}
void obj_list::build_bvh(bool print, uint node_size) {
	obj_unroll();
	bvh.reserve(obj_bvh.size());
	split_bvh(0, obj_bvh.size(), node_size);
	if (print)
		printf("\n%d\n", (int)bvh.size());
}
void obj_list::split_bvh(uint be, uint en, uint node_size) {
	aabb bbox = box_from(be, en);
	uint size = en - be;
	uchar axis = bbox.get_longest_axis();
	auto begin = obj_bvh.begin() + be;
	auto end = obj_bvh.begin() + en;
	switch (axis) {
	case 0: std::sort(begin, end, cmp_axis_x); break;
	case 1: std::sort(begin, end, cmp_axis_y); break;
	default: std::sort(begin, end, cmp_axis_z); break;
	}
	if (size <= node_size)bvh.push_back(bvh_node(bbox, be, en, 0));
	else {
		uint n = bvh.size();
		bvh.push_back(bvh_node(bbox, n + 1, 0, 1));
		split_bvh(be, be + size / 2, node_size);
		bvh[n].n2 = bvh.size();
		split_bvh(be + size / 2, be + size, node_size);
	}
}

aabb obj_list::box_from(uint begin_l, uint end) {
	aabb bbox = aabb();
	for (uint i = begin_l; i < end; i++)
	{
		bbox.join(obj_bvh[i].get_box());
	}
	return bbox;
}