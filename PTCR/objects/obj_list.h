#pragma once
#include "obj.h"
#include "mesh.h"

class obj_list {
public:
	obj_list() {}
	~obj_list() { clear(); }
	//Definitions

	__forceinline bool hit(const ray& r, hitrec& rec) const {

		if (!bbox.hit(r)) return false;
		if (en_bvh)return hit_bvh(r, rec, 0);
		bool hit = false;
		for (auto& obj : objects)hit |= obj.hit(r, rec);
		return hit;
	}
	__forceinline float pdf(const ray& r)const {
		if (!bbox.hit(r))return 0;
		if (lights.size() == 1)return objects[lights[0]].pdf(r);
		float y = 0.f;
		for (const auto& light : lights)
			y += lw * objects[light].pdf(r);
		return y;
	}
	__forceinline vec3 rand_to(vec3 O) const {
		if (lights.size() == 1)return objects[lights[0]].rand_to(O);
		uint id = raint(lights.size() - 1);
		const uint light = lights[id];
		return objects[light].rand_to(O);
	}
	__forceinline vec3 rand_from() const {
		if (lights.size() == 1)return objects[lights[0]].rand_from();
		uint id = raint(lights.size() - 1);
		uint light = lights[id];
		return objects[light].rand_from();
	}
	// 0th and last + 1 element
	__forceinline bool hit_bvh(const ray& r, hitrec& rec, uint id = 0) const {
		const bvh_node& node = bvh[id];
		if (node.flag)
		{
			bool h1 = bvh[node.n1].bbox.hit(r);
			bool h2 = bvh[node.n2].bbox.hit(r);
			if (h1 && h2) return hit_bvh(r, rec, node.n1) + hit_bvh(r, rec, node.n2);
			else if (h1)return hit_bvh(r, rec, node.n1);
			else if (h2)return hit_bvh(r, rec, node.n2);
			else return false;
		}
		else{
			bool hit = 0;
			for (uint i = node.n1; i < node.n2; i++)
				hit |= obj_bvh[i].hit(r, rec);
			return hit;
		}
	}
	void rebuild_bvh(bool print = 0, uint node_size = 8) {
		bvh.clear();
		obj_bvh.clear();
		build_bvh(print, node_size);
	}
	void obj_unroll() {
		obj_bvh.reserve(4 * objects.size());
		for (const auto& obj : objects)
		{
			switch (obj.id) {
			case o_sph: {
				vector<sphere> prim = obj.s.get_data();
				for (const auto& pr : prim)
					obj_bvh.push_back(mesh_raw(pr, obj.get_mat()));
				break;
			}
			case o_qua: {
				vector<quad> prim = obj.q.get_data();
				for (const auto& pr : prim)
					obj_bvh.push_back(mesh_raw(pr, obj.get_mat()));
				break;
			}
			case o_tri: {
				vector<tri> prim = obj.t.get_data();
				for (const auto& pr : prim)
					obj_bvh.push_back(mesh_raw(pr, obj.get_mat()));
				break;
			}
			case o_vox: {
				vector<voxel> prim = obj.v.get_data();
				for (const auto& pr : prim)
					obj_bvh.push_back(mesh_raw(pr, obj.get_mat()));
				break;
			}
			default:break;
			}

		}
	}
	void build_bvh(bool print = 1, uint node_size = 8) {
		obj_unroll();
		bvh.reserve(2 * obj_bvh.size());
		split_bvh(0, obj_bvh.size(), node_size);
		if (print)
			printf("\n%d\n", (int)bvh.size());
	}

	void split_bvh(uint be, uint en, uint node_size) {
		aabb bbox = box_from(be, en);
		uint size = en - be;
		uint axis = bbox.get_longest_axis();
		auto begin = obj_bvh.begin() + be;
		auto end = obj_bvh.begin() + en;
		if (axis == 0)std::sort(begin, end, cmp_axis_x);
		else if (axis == 1)std::sort(begin, end, cmp_axis_y);
		else std::sort(begin, end, cmp_axis_z);
		if (size <= node_size)bvh.push_back(bvh_node(bbox, be, en, 0));
		else {
			uint n = bvh.size();
			bvh.push_back(bvh_node(bbox, n + 1, 0, 1));
			split_bvh(be, be + size / 2, node_size);
			bvh[n].n2 = bvh.size();
			split_bvh(be + size / 2, be + size, node_size);
		}
	}
	aabb box_from(uint begin, uint end) {
		aabb bbox = aabb();
		for (uint i = begin; i < end; i++)
		{
			bbox.join(obj_bvh[i].get_box());
		}
		return bbox;
	}
	//Declarations
	template <typename T>
	void add(const T& object, uint mat, bool is_light = 0);
	template <typename T>
	void add(const vector<T>& object, uint mat, bool is_light = 0);
	template <typename T>
	void add(vec3 off, const T& object, uint mat, bool is_light = 0);
	template <typename T>
	void add(vec3 off, const vector<T>& object, uint mat, bool is_light = 0);
	void clear();
	obj_id get_id(const ray& r, hitrec& rec) const;
	void get_trans(const obj_id id, matrix& T) const;
	void set_trans(const obj_id id, const matrix& T, uint node_size = 8);
	void fit();
	void add_mat(const albedo& tex, const mat_enum type) {
		add_mat(mat_var(tex, type));
	}
	void add_mat(const mat_var& mat) {
		materials.emplace_back(mat);
	}
private:
	static bool cmp_axis_x(const mesh_raw& a, const mesh_raw& b) {
		return a.get_box().pmax.x < b.get_box().pmax.x;
	};
	static bool cmp_axis_y(const mesh_raw& a, const mesh_raw& b) {
		return a.get_box().pmax.y < b.get_box().pmax.y;
	};
	static bool cmp_axis_z(const mesh_raw& a, const mesh_raw& b) {
		return a.get_box().pmax.z < b.get_box().pmax.z;
	};
	template <typename T>
	void add(const mesh<T>& object, bool is_light)
	{
		bbox.join(object.get_box());
		objects.emplace_back(object);
		if (is_light) lights.emplace_back(objects.size() - 1), lw = 1.f / lights.size();
	}
	template <typename T>
	void add(vec3 off, const mesh<T>& object, bool is_light)
	{
		mesh<T> tmp = object;
		tmp.set_trans(matrix(off, 0));
		bbox.join(tmp.get_box());
		objects.emplace_back(tmp);
		if (is_light) lights.emplace_back(objects.size() - 1), lw = 1.f / lights.size();
	}
public:
	aabb bbox;
	vector<uint> lights;
	vector<mesh_var> objects;
	vector<mesh_raw> obj_bvh;
	vector<bvh_node> bvh;
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

template <typename T>
void obj_list::add(vec3 off, const T& object, uint mat, bool is_light)
{
	add(off, mesh<T>(object, mat), is_light);
}
template <typename T>
void obj_list::add(vec3 off, const vector<T>& object, uint mat, bool is_light)
{
	add(off, mesh<T>(object, mat), is_light);
}

