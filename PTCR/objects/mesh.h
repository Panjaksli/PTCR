#pragma once
#include "sphere.h"
#include "tri.h"
#include "quad.h"
#include "voxel.h"


#pragma pack(push, 4)
template <class primitive>
class mesh {
public:
	mesh() {}
	mesh(const primitive& _prim, uint _mat) :prim(new primitive[1]{ _prim }), mat(_mat), size(1), lw(1.f) {
		fit();
	}
	mesh(const vector<primitive>& _prim, uint _mat) :prim(new primitive[_prim.size()]), mat(_mat), size(_prim.size()), lw(1.f / size) {
		memcpy(prim, &_prim[0], size * sizeof(primitive));
		fit();
	}
	mesh(const mesh& cpy) : bbox(cpy.bbox), P(cpy.P), A(cpy.A), prim(new primitive[cpy.size]), mat(cpy.mat), size(cpy.size), lw(cpy.lw) {
		memcpy(prim, cpy.prim, size * sizeof(primitive));
	}
	const mesh& operator=(const mesh& cpy) {
		bbox = cpy.bbox, P = cpy.P, A = cpy.A, prim = new primitive[cpy.size], mat = cpy.mat, size = cpy.size, lw = cpy.lw;
		memcpy(prim, cpy.prim, size * sizeof(primitive));
		return *this;
	}
	~mesh() {
		clean();
	}
	__forceinline bool hit(const ray& r, hitrec& rec) const
	{
		if (size == 1)
		{
			bool hit = prim[0].hit(r, rec);
			if (hit)rec.mat = mat;
			return hit;
		}
		bool hit = false;
		for (uint i = 0; i < size; i++)
			hit |= prim[i].hit(r, rec);
		if (hit)rec.mat = mat;
		return hit;
	}
	inline matrix get_trans()const {
		return matrix(P, A);
	}
	inline void set_trans(const matrix& _T) {
		matrix dT1(-P);
		matrix dT2(0, _T.A - A);
		matrix dT3(_T.P(), 0);
		P = _T.P();
		A = _T.A;
		transform(dT1);
		transform(dT2);
		build_cache(dT3);
	}
	inline aabb get_box()const { return bbox; }
	__forceinline float pdf(const ray& r)const {
		if (size == 1) return prim[0].pdf(r);
		float y = 0.f;
		for (uint i = 0; i < size; i++)
		{
			y += lw * prim[i].pdf(r);
		}
		return y;
	}
	__forceinline vec3 rand_to(vec3 O) const {
		if (size == 1) return prim[0].rand_to(O);
		uint id = raint(size - 1);
		return prim[id].rand_to(O);
	}
	__forceinline vec3 rand_from() const {
		if (size == 1) return prim[0].rand_from();
		uint id = raint(size - 1);
		return prim[id].rand_from();
	}
	inline vector<primitive> get_data()const {
		return vector<primitive>(prim, prim + size);
	}
	inline primitive* get_data(uint i) {
		return &(prim[i]);
	}
	inline uint get_mat()const {
		return mat;
	}
	inline uint get_size()const {
		return size;
	}
private:
	inline void transform(const matrix& dT) {
		for (uint i = 0; i < size; i++)
			prim[i] = prim[i].trans(dT);
	}
	inline void build_cache(const matrix& dT) {
		transform(dT);
		fit();
	}
	inline void fit() {
		bbox = aabb();
		for (uint i = 0; i < size; i++)
		{
			bbox.join(prim[i].get_box());
		}
	}
	inline void clean() {
		delete[]prim;
	}

	aabb bbox;
	vec3 P, A;
	primitive* prim;
	uint mat;
	uint size;
	float lw;
};

using tmesh = mesh<tri>;
using qmesh = mesh<quad>;
using smesh = mesh<sphere>;
using vmesh = mesh<voxel>;


struct mesh_var {
	mesh_var(const smesh& m) :s(m), id(o_sph) {}
	mesh_var(const qmesh& m) :q(m), id(o_qua) {}
	mesh_var(const tmesh& m) :t(m), id(o_tri) {}
	mesh_var(const vmesh& m) :v(m), id(o_vox) {}
	mesh_var(const mesh_var& cpy) :id(cpy.id) {
		switch (id) {
		case o_sph: s = cpy.s; break;
		case o_qua: q = cpy.q; break;
		case o_tri: t = cpy.t; break;
		case o_vox:	v = cpy.v; break;
		

		default:break;
		}
	}
	const mesh_var& operator=(const mesh_var& cpy) {
		id = cpy.id;
		switch (id) {
		case o_sph: s = cpy.s; break;
		case o_qua: q = cpy.q; break;
		case o_tri: t = cpy.t; break;
		case o_vox:	v = cpy.v; break;
		

		default:break;
		}
		return *this;
	}
	~mesh_var() {
		//Destructor DOES matter here (each type holds a different pointer, thus correct delete[] method shall be called !)
		switch (id) {
		case o_sph: s.~mesh(); break;
		case o_qua: q.~mesh(); break;
		case o_tri: t.~mesh(); break;
		case o_vox:	v.~mesh(); break;
		

		default:break;
		}
	}
	inline uint get_mat()const {
		return s.get_mat();
	}
	__forceinline bool hit(const ray& r, hitrec& rec) const
	{
		if (!s.get_box().hit(r))return false;
		switch (id) {
		case o_sph: return s.hit(r, rec);
		case o_qua: return q.hit(r, rec);
		case o_tri: return t.hit(r, rec);
		case o_vox: return v.hit(r, rec);
		

		default: return false;
		}
	}

	matrix get_trans()const {
		switch (id) {
		case o_sph: return s.get_trans();
		case o_qua: return q.get_trans();
		case o_tri: return t.get_trans();
		case o_vox: return v.get_trans();
		

		default: return matrix();
		}
	}
	void set_trans(const matrix& T) {
		switch (id) {
		case o_sph: s.set_trans(T); break;
		case o_qua: q.set_trans(T); break;
		case o_tri: t.set_trans(T); break;
		case o_vox: v.set_trans(T); break;
		

		default: break;
		}
	}
	inline uint get_size()const {
		return s.get_size();
	}
	inline aabb get_box()const {
		return s.get_box();
	}
	__forceinline float pdf(const ray& r)const {
		if (!s.get_box().hit(r))return 0;
		switch (id) {
		case o_sph: return s.pdf(r);
		case o_qua: return q.pdf(r);
		case o_tri: return t.pdf(r);
		case o_vox: return v.pdf(r);
		

		default: return 0;
		}
	}
	__forceinline vec3 rand_to(vec3 O) const {
		switch (id) {
		case o_sph: return s.rand_to(O);
		case o_qua: return q.rand_to(O);
		case o_tri: return t.rand_to(O);
		case o_vox: return v.rand_to(O);
		

		default: return 0;
		}
	}
	__forceinline vec3 rand_from() const {
		switch (id) {
		case o_sph: return s.rand_from();
		case o_qua: return q.rand_from();
		case o_tri: return t.rand_from();
		case o_vox: return v.rand_from();
		

		default: return 0;
		}
	}
	union {
		smesh s;
		qmesh q;
		tmesh t;
		vmesh v;
	};
	obj_enum id;
};


struct mesh_raw {
	mesh_raw(tri* m, uint mat) : bbox(m->get_box()), t(m), mat(mat), id(o_tri) {}
	mesh_raw(quad* m, uint mat) : bbox(m->get_box()), q(m), mat(mat), id(o_qua) {}
	mesh_raw(sphere* m, uint mat) : bbox(m->get_box()), s(m), mat(mat), id(o_sph) {}
	mesh_raw(voxel* m, uint mat) :bbox(m->get_box()), v(m), mat(mat), id(o_vox) {}
	mesh_raw(const mesh_raw& cpy) :bbox(cpy.bbox),t(cpy.t), mat(cpy.mat), id(cpy.id) {	}
	const mesh_raw& operator=(const mesh_raw& cpy) {
		bbox = cpy.bbox;
		t = cpy.t;
		mat = cpy.mat;
		id = cpy.id;
		return *this;
	}

	__forceinline bool hit(const ray& r, hitrec& rec) const
	{
		if (!bbox.hit(r))return false;
		bool hit = 0;
		switch (id) {
		case o_tri: hit = t->hit(r, rec); break;
		case o_qua: hit = q->hit(r, rec); break;
		case o_sph: hit = s->hit(r, rec); break;
		case o_vox:	hit = v->hit(r, rec); break;
		default:break;
		}
		if (hit)rec.mat = mat;
		return hit;
	}

	inline aabb get_box()const {
		return bbox;
	}

	aabb bbox;
	union {
		tri* t;
		quad* q;
		sphere* s;
		voxel* v;
	};
	uint mat;
	obj_enum id;
};
#pragma pack(pop)


