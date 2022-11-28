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
		bool hit = false;
		for (auto& obj : skp_bvh)hit |= obj.hit(r, rec);
		if (en_bvh) {
			if (!bvh[0].bbox.hit(r))return hit;
			return hit + hit_bvh(r, rec, 0);
		}
		else
		{
			for (auto& obj : objects)hit |= obj.hit(r, rec);
		}
		return hit;
	}
	__forceinline float pdf(const ray& r)const {
		float y = 0.f;
		for (const auto& light : lights)
			y += lw * skp_bvh[light].pdf(r);
		return y;
	}
	__forceinline vec3 rand_to(vec3 O) const {
		uint id = raint(lights.size() - 1);
		const uint light = lights[id];
		return skp_bvh[light].rand_to(O);
	}
	__forceinline vec3 rand_from() const {
		uint id = raint(lights.size() - 1);
		uint light = lights[id];
		return skp_bvh[light].rand_from();
	}
	// 0th and last + 1 element
	__forceinline bool hit_bvh(const ray& r, hitrec& rec, uint id = 0) const {
		const bvh_node& node = bvh[id];
		if (node.flag)
		{
#if 0
			bool h1 = bvh[node.n1].bbox.hit(r);
			bool h2 = bvh[node.n2].bbox.hit(r);
			if (h1 && h2)return hit_bvh(r, rec, node.n1) + hit_bvh(r, rec, node.n2);
			else if (h1)return hit_bvh(r, rec, node.n1);
			else if (h2)return hit_bvh(r, rec, node.n2);
			else return false;
#else
			//second approach is faster... just because of better memory locality ? program jumps to nodes in the right order, first the closer node, then the further
			//however trying to be extra smart and stopping the recursion at the closest intersection within node, causes program to go haywire and miss all the nodes (still not sure why)
		float t1 = bvh[node.n1].bbox.hit(r, rec.t);
		float t2 = bvh[node.n2].bbox.hit(r, rec.t);
		if (t1 < infp && t2 < infp) {
			if (t1 < t2) return hit_bvh(r, rec, node.n1) + hit_bvh(r, rec, node.n2);	
			else return hit_bvh(r, rec, node.n2) + hit_bvh(r, rec, node.n1);
		}
		else if (t1 < infp)return hit_bvh(r, rec, node.n1);
		else if (t2 < infp)return hit_bvh(r, rec, node.n2);
		else return false;
#endif
		}
		else {
			bool hit = 0;
			for (uint i = node.n1; i < node.n2; i++)
				hit |= obj_bvh[i].hit(r, rec);
			return hit;
		}
	}
	void add_mat(const albedo& tex, const mat_enum type) {
		add_mat(mat_var(tex, type));
	}
	void add_mat(const mat_var& mat) {
		materials.emplace_back(mat);
	}
	//Declarations
	template <typename T>
	void add(const T& object, uint mat, bool skip_bvh = 0, bool is_light = 0);
	template <typename T>
	void add(const vector<T>& object, uint mat, bool skip_bvh = 0, bool is_light = 0);
	template <typename T>
	void add(vec3 off, const T& object, uint mat, bool skip_bvh = 0, bool is_light = 0);
	template <typename T>
	void add(vec3 off, const vector<T>& object, uint mat, bool skip_bvh = 0, bool is_light = 0);
	void clear();
	obj_id get_id(const ray& r, hitrec& rec) const;
	void get_trans(const obj_id id, matrix& T) const;
	void set_trans(const obj_id id, const matrix& T, uint node_size = 8);
	void fit();
	void rebuild_bvh(bool print = 0, uint node_size = 8);
	void obj_unroll();
	void build_bvh(bool print = 1, uint node_size = 8);
	void split_bvh(uint be, uint en, uint node_size);
	aabb box_from(uint begin, uint end);



private:

	template <typename T>
	void add(const mesh<T>& object, bool skip_bvh = 0, bool is_light = 0)
	{
		if (is_light || skip_bvh) {
			skp_bvh.emplace_back(object);
			if (is_light) lights.emplace_back(skp_bvh.size() - 1), lw = 1.f / lights.size();
		}
		else {
			objects.emplace_back(object);
		}
		bbox.join(object.get_box());
	}
	template <typename T>
	void add(vec3 off, const mesh<T>& object, bool skip_bvh = 0, bool is_light = 0)
	{
		mesh<T> tmp = object;
		tmp.set_trans(matrix(off, 0));
		add(tmp, skip_bvh, is_light);
	}
	static bool cmp_axis_x(const mesh_raw& a, const mesh_raw& b) {
		return a.get_box().pmid().x() < b.get_box().pmid().x();
	};
	static bool cmp_axis_y(const mesh_raw& a, const mesh_raw& b) {
		return a.get_box().pmid().y() < b.get_box().pmid().y();
	};
	static bool cmp_axis_z(const mesh_raw& a, const mesh_raw& b) {
		return a.get_box().pmid().z() < b.get_box().pmid().z();
	};
public:
	aabb bbox;
	vector<uint> lights;
	vector<mesh_var> skp_bvh;
	vector<mesh_var> objects;
	vector<mesh_raw> obj_bvh;
	vector<bvh_node> bvh;
	vector<mat_var> materials;
	float lw;
};
template <typename T>
void obj_list::add(const T& object, uint mat, bool skip_bvh, bool is_light)
{
	add(mesh<T>(object, mat), skip_bvh, is_light);
}
template <typename T>
void obj_list::add(const vector<T>& object, uint mat, bool skip_bvh, bool is_light)
{
	add(mesh<T>(object, mat), skip_bvh, is_light);
}

template <typename T>
void obj_list::add(vec3 off, const T& object, uint mat, bool skip_bvh, bool is_light)
{
	add(off, mesh<T>(object, mat), skip_bvh, is_light);
}
template <typename T>
void obj_list::add(vec3 off, const vector<T>& object, uint mat, bool skip_bvh, bool is_light)
{
	add(off, mesh<T>(object, mat), skip_bvh, is_light);
}

