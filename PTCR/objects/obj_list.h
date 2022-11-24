#pragma once
#include "obj.h"
#include "mesh.h"
extern bool en_bvh;
class obj_list {
public:
	obj_list() {}
	~obj_list() { clear(); }
	//Definitions

	__forceinline bool hit(const ray& r, hitrec& rec) const {

		if (!bbox.hit(r)) return false;
		if (en_bvh)return hit_bvh(r, rec, 0);
		bool hit = false;
		for (auto& obj : bvh_obj)hit |= obj.hit(r, rec);
		return hit;
	}
	__forceinline bool hit_id(const ray& r, hitrec& rec, uint id) const {
		return objects[id].hit(r, rec);
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
	aabb box_from(uint begin, uint end) {
		aabb bbox = aabb();
		for (uint i = begin; i < end; i++)
		{
			bbox.join(bvh_obj[i].get_box());
		}
		return bbox;
	}

	void rebuild_bvh() {
		bvh.clear();
		build_bvh();
	}
	void build_bvh() {
		bvh_obj = objects;
		bvh.reserve(2 * bvh_obj.size());
		split_bvh(0, bvh_obj.size());
		//printf("\n%d", (int)bvh.size());
	}

#if node4
	__forceinline bool hit_bvh(const ray& r, hitrec& rec, uint id = 0) const {
		const bvh_node& node = bvh[id];
		uchar flag = node.flag;
		if (flag > 4)
		{
			bool h1 = bvh[node.n1].bbox.hit(r);
			bool h2 = bvh[node.n2].bbox.hit(r);
			bool h3 = bvh[node.n3].bbox.hit(r);
			bool h4 = bvh[node.n4].bbox.hit(r);
			bool res = 0;
			res |= h1 && hit_bvh(r, rec, node.n1);
			res |= h2 && hit_bvh(r, rec, node.n2);
			res |= h3 && hit_bvh(r, rec, node.n3);
			res |= h4 && hit_bvh(r, rec, node.n4);
			return res;
		}
		else if (flag <= 1)return bvh_obj[node.n1].hit(r, rec);
		else if (flag <= 2)return bvh_obj[node.n1].hit(r, rec) + bvh_obj[node.n2].hit(r, rec);
		else if (flag <= 3)return bvh_obj[node.n1].hit(r, rec) + bvh_obj[node.n2].hit(r, rec) + bvh_obj[node.n3].hit(r, rec);
		else if (flag <= 4)return bvh_obj[node.n1].hit(r, rec) + bvh_obj[node.n2].hit(r, rec) + bvh_obj[node.n3].hit(r, rec) + bvh_obj[node.n4].hit(r, rec);
		else return false;
	}

	void split_bvh(uint be, uint en) {
		aabb bbox = box_from(be, en);
		uint size = en - be;
		uint axis = bbox.get_longest_axis();
		auto begin = bvh_obj.begin() + be;
		auto end = bvh_obj.begin() + en;
		if (axis == 0)std::sort(begin, end, cmp_axis_x);
		else if (axis == 1)std::sort(begin, end, cmp_axis_y);
		else std::sort(begin, end, cmp_axis_z);
		if (size <= 4)bvh.push_back(bvh_node(bbox, be, be + 1, be + 2, be + 3, size));
		else {
			uint n = bvh.size();
			bvh.push_back(bvh_node(bbox, n + 1, 0, 0, 0, 5));
			split_bvh(be, be + size / 4);
			bvh[n].n2 = bvh.size();
			split_bvh(be + size / 4, be + 2 * size / 4);
			bvh[n].n3 = bvh.size();
			split_bvh(be + 2 * size / 4, be + 3 * size / 4);
			bvh[n].n4 = bvh.size();
			split_bvh(be + 3 * size / 4, be + 4 * size / 4);
		}
	}

#else

	__forceinline bool hit_bvh(const ray& r, hitrec& rec, uint id = 0) const {
		const bvh_node& node = bvh[id];
		uchar flag = node.flag;
		if (flag > 2)
		{
			bool h1 = bvh[node.n1].bbox.hit(r);
			bool h2 = bvh[node.n2].bbox.hit(r);
			if (h1 && h2) return hit_bvh(r, rec, node.n1) + hit_bvh(r, rec, node.n2);
			else if (h1)return hit_bvh(r, rec, node.n1);
			else if (h2)return hit_bvh(r, rec, node.n2);
			else return false;
		}
		else if (flag <= 0)return false;
		else if (flag <= 1)return bvh_obj[node.n1].hit(r, rec);
		else if (flag <= 2)return bvh_obj[node.n1].hit(r, rec) + bvh_obj[node.n2].hit(r, rec);
		else return false;

	}
	void split_bvh(uint be, uint en) {
		aabb bbox = box_from(be, en);
		uint size = en - be;
		uint axis = bbox.get_longest_axis();
		auto begin = bvh_obj.begin() + be;
		auto end = bvh_obj.begin() + en;
		if (axis == 0)std::sort(begin, end, cmp_axis_x);
		else if (axis == 1)std::sort(begin, end, cmp_axis_y);
		else std::sort(begin, end, cmp_axis_z);
		if (size <= 2)bvh.push_back(bvh_node(bbox, be, be + 1, size));
		else {
			uint n = bvh.size();
			bvh.push_back(bvh_node(bbox, n + 1, 0, 3));
			split_bvh(be, be + size / 2);
			bvh[n].n2 = bvh.size();
			split_bvh(be + size / 2, be + size);
		}
	}

#endif


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
	void add_mat(const albedo& tex, const mat_enum type) {
		add_mat(mat_var(tex, type));
	}
	void add_mat(const mat_var& mat) {
		materials.emplace_back(mat);
	}
private:
	static bool cmp_axis_x(const mesh_var& a, const mesh_var& b) {
		return a.get_box().pmax.x < b.get_box().pmax.x;
	};
	static bool cmp_axis_y(const mesh_var& a, const mesh_var& b) {
		return a.get_box().pmax.y < b.get_box().pmax.y;
	};
	static bool cmp_axis_z(const mesh_var& a, const mesh_var& b) {
		return a.get_box().pmax.z < b.get_box().pmax.z;
	};
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
	vector<mesh_var> bvh_obj;
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

