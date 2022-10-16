#pragma once
#include "sphere.h"
#include "tri.h"
#include "quad.h"
#include "voxel.h"
#pragma pack(push, 8)
template <class primitive>
class mesh {
public:
	mesh() {}
	mesh(const primitive &_prim, sptr<material> _mat) :prim(new primitive(_prim)), mat(_mat.get()), size(1), lw(1.f) {
		fit();
	}
	mesh(const vector<primitive> &_prim, sptr<material> _mat) :prim(new primitive[_prim.size()]), mat(_mat.get()), size(_prim.size()), lw(1.f / size) {
		for (uint i = 0; i < _prim.size(); i++)
			prim[i] = _prim[i];
		fit();
	}
	inline bool hit(const ray& r, hitrec& rec) const
	{
		if (size == 1)
		{
			bool hit = prim->hit(r, rec);
			if(hit)rec.mat = mat;
			return hit;
		}
		bool hit = false;
		for (uint i = 0; i < size; i++)
			hit |= prim[i].hit(r, rec);
		if(hit)rec.mat = mat;
		return hit;
	}
	inline matrix get_trans()const {
		return matrix(P,A);
	}
	inline void set_trans(const matrix& _T) {
		matrix dT(P-_T.P(),A-_T.A);
		P = _T.P();
		A = _T.A;
		build_cache(dT);
	}
	inline aabb get_box()const { return bbox; }
	inline float pdf(const ray& r)const {
		if (size == 1) return prim[0].pdf(r);
		float y = 0.f;
		for (uint i = 0; i < size; i++)
		{
			y += prim[i].pdf(r);
		}
		return lw * y;
	}
	inline vec3 rand_to(vec3 O) const {
		if (size == 1) return prim->rand_to(O);
		uint id = raint(size - 1);
		return prim[id].rand_to(O);
	}
	inline vec3 rand_from() const {
		if (size == 1) return prim->rand_from();
		uint id = raint(size - 1);
		return prim[id].rand_from();
	}
	inline void free() {
		delete[]prim;
	}
private:
	inline void build_cache(const matrix& dT) {
		for (uint i = 0; i < size; i++)
			prim[i] = prim[i].trans(dT);
		fit();
	}
	inline void fit() {
		bbox = aabb();
		for (uint i = 0; i < size; i++)
		{
			bbox.join(prim[i].get_box());
		}
	}
	aabb bbox;
	vec3 P, A;
	primitive* prim;
	material* mat;
	uint size;
	float lw;
};

using tmesh = mesh<tri>;
using qmesh = mesh<quad>;
using smesh = mesh<sphere>;
using vmesh = mesh<voxel>;


struct mesh_var {
	mesh_var(const smesh &m) :s(m), id(o_sph) {}
	mesh_var(const qmesh &m) :q(m), id(o_qua) {}
	mesh_var(const tmesh &m) :t(m), id(o_tri) {}
	mesh_var(const vmesh &m) :v(m), id(o_vox) {}
	inline bool hit(const ray& r, hitrec& rec) const
	{
		if (!s.get_box().hit(r))return false;
		switch (id) {
		case o_sph: return s.hit(r, rec);
		case o_qua: return q.hit(r, rec);
		case o_tri: return t.hit(r, rec);
		case o_vox: return v.hit(r, rec);
		case o_bla:
		case o_bvh:
		default: return false;
		}
	}
	matrix get_trans()const {
		switch (id) {
		case o_sph: return s.get_trans();
		case o_qua: return q.get_trans();
		case o_tri: return t.get_trans();
		case o_vox: return v.get_trans();
		case o_bla:
		case o_bvh:
		default: return matrix();
		}
	}
	void set_trans(const matrix& T) {
		switch (id) {
		case o_sph: s.set_trans(T); break;
		case o_qua: q.set_trans(T); break;
		case o_tri: t.set_trans(T); break;
		case o_vox: v.set_trans(T); break;
		case o_bla:
		case o_bvh:
		default: break;
		}
	}
	inline aabb get_box()const {
		return s.get_box();
	}
	inline float pdf(const ray& r)const {
		if (!s.get_box().hit(r))return 0;
		switch (id) {
		case o_sph: return s.pdf(r);
		case o_qua: return q.pdf(r);
		case o_tri: return t.pdf(r);
		case o_vox: return v.pdf(r);
		case o_bla:
		case o_bvh:
		default: return 0;
		}
	}
	inline vec3 rand_to(vec3 O) const {
		switch (id) {
		case o_sph: return s.rand_to(O);
		case o_qua: return q.rand_to(O);
		case o_tri: return t.rand_to(O);
		case o_vox: return v.rand_to(O);
		case o_bla:
		case o_bvh:
		default: return 0;
		}
	}
	inline vec3 rand_from() const {
		switch (id) {
		case o_sph: return s.rand_from();
		case o_qua: return q.rand_from();
		case o_tri: return t.rand_from();
		case o_vox: return v.rand_from();
		case o_bla:
		case o_bvh:
		default: return 0;
		}
	}
	inline void free() {
		switch (id) {
		case o_sph: s.free(); break;
		case o_qua: q.free(); break;
		case o_tri: t.free(); break;
		case o_vox:	v.free(); break;
		case o_bla:
		case o_bvh:
		default:break;
		}
	}
	union {
		smesh s;
		qmesh q;
		tmesh t;
		vmesh v;
	};
	const obj_enum id;
};
#pragma pack(pop)