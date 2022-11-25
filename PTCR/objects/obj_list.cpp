#include "obj_list.h"
void obj_list::clear()
{
	bvh.clear();
	obj_bvh.clear();
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
		if (objects[i].hit(r, rec))res = obj_id(objects[i].id, i);
	return res;
}
void obj_list::get_trans(const obj_id id, matrix& T)const
{
	T = objects[id.id].get_trans();
}
void obj_list::set_trans(obj_id id, const matrix& T, uint node_size) {
	objects[id.id].set_trans(T);
	fit();
	rebuild_bvh(0,node_size);
}
void obj_list::fit() {
	bbox = aabb();
	for (const auto& obj : objects)
		bbox.join(obj.get_box());
}