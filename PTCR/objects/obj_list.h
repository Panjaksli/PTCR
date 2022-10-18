#pragma once
#include "obj.h"
#include "mesh.h"
class obj_list {
public:
	obj_list() {}
	~obj_list() { clear(); }
	//Definitions
	inline aabb get_box(const obj_id id) const {
		return objects[id.id].get_box();
	}

	inline bool hit(const ray& r, hitrec& rec) const {
		if (!bbox.hit(r)) return false;
		bool hit = false;
		for (auto& obj : objects)hit |= obj.hit(r, rec);
		return hit;
	}

	inline float pdf(const ray& r)const {
		if (!bbox.hit(r))return 0;
		if (lights.size() == 1)return objects[lights[0]].pdf(r);
		float y = 0.f;
		for (const auto& light : lights)
			y += objects[light].pdf(r);
		return lw * y;
	}
	inline vec3 rand_to(vec3 O) const {
		if (lights.size() == 1)return objects[lights[0]].rand_to(O);
		uint id = raint(lights.size() - 1);
		const uint light = lights[id];
		return objects[light].rand_to(O);
	}
	inline vec3 rand_from() const {
		if (lights.size() == 1)return objects[lights[0]].rand_from();
		uint id = raint(lights.size() - 1);
		uint light = lights[id];
		return objects[light].rand_from();
	}
	//Declarations
	template <typename T>
	void add(const T& object, uint mat, bool is_light = 0);
	template <typename T>
	void add(const vector<T>& object, uint mat, bool is_light = 0);
	void clear();
	obj_id get_id(const ray& r, hitrec& rec) const;
	void get_trans(const obj_id id, matrix& T) const;
	void set_trans(const obj_id id, const matrix& T);
	void fit();
	template <typename T>
	void add_mat(const T &mat) {
		materials.emplace_back(mat);
	}
private:
	template <typename T>
	void add(const mesh<T>& object, bool is_light)
	{
		bbox.join(object.get_box());
		objects.emplace_back(object);
		if (is_light) lights.emplace_back(objects.size() - 1), lw = 1.f / lights.size();
	}
public:
	aabb bbox;
	vector<uint> lights;
	vector<mesh_var> objects;
	vector<mat_var> materials;
	float lw;
};
template <typename T>
void obj_list::add(const T& object, uint mat, bool is_light)
{
	add(mesh<T>(object, mat), is_light);
}
template <typename T>
void obj_list::add(const vector<T>& object, uint mat, bool is_light)
{
	add(mesh<T>(object, mat), is_light);
}

