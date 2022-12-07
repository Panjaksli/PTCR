#pragma once
#include "sphere.h"
#include "poly.h"
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
		bool any_hit = false;
		for (uint i = 0; i < size; i++)
			any_hit |= prim[i].hit(r, rec);
		rec.mat = any_hit ? mat : rec.mat;
		return any_hit;
	}

	__forceinline bool hit(const ray& r, hitrec& rec, uint prim_id) const
	{
		bool any_hit = prim[prim_id].hit(r, rec);
		rec.mat = any_hit ? mat : rec.mat;
		return any_hit;
	}

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

	inline aabb get_box()const { return bbox; }
	inline aabb get_box(uint prim_id)const { return prim[prim_id].get_box(); }
	inline vector<primitive> get_data()const {
		return vector<primitive>(prim, prim + size);
	}
	inline primitive* get_data(uint i) const {
		return &(prim[i]);
	}
	inline uint get_mat()const {
		return mat;
	}
	inline uint get_size()const {
		return size;
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
#pragma omp parallel for schedule(static,64)
		for (uint i = 0; i < size; i++)
		{
			prim[i] = prim[i].trans(dT1);
			prim[i] = prim[i].trans(dT2);
			prim[i] = prim[i].trans(dT3);
		}
		fit();
	}
private:
	inline void transform(const matrix& dT) {
#pragma omp parallel for schedule(static,64)
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
			bbox.join(prim[i].get_box());
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

using tmesh = mesh<poly>;
using qmesh = mesh<quad>;
using smesh = mesh<sphere>;
using vmesh = mesh<voxel>;


struct mesh_var {
	mesh_var(const mesh<poly>& m) :p(m), id(o_pol) {}
	mesh_var(const mesh<quad>& m) :q(m), id(o_qua) {}
	mesh_var(const mesh<sphere>& m) :s(m), id(o_sph) {}
	mesh_var(const mesh<voxel>& m) :v(m), id(o_vox) {}
	mesh_var(const mesh_var& cpy) :id(cpy.id) {
		switch (id) {
		case o_pol: p = cpy.p; break;
		case o_qua: q = cpy.q; break;
		case o_sph: s = cpy.s; break;
		case o_vox:	v = cpy.v; break;
		default:break;
		}
	}
	const mesh_var& operator=(const mesh_var& cpy) {
		id = cpy.id;
		switch (id) {
		case o_pol: p = cpy.p; break;
		case o_qua: q = cpy.q; break;
		case o_sph: s = cpy.s; break;
		case o_vox:	v = cpy.v; break;
		default:break;
		}
		return *this;
	}
	~mesh_var() {
		//Destructor DOES matter here (each type holds a different pointer, thus correct delete[] method shall be called !)
		switch (id) {
		case o_pol: p.~mesh(); break;
		case o_qua: q.~mesh(); break;
		case o_sph: s.~mesh(); break;
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
		case o_pol: return p.hit(r, rec);
		case o_qua: return q.hit(r, rec);
		case o_sph: return s.hit(r, rec);
		case o_vox: return v.hit(r, rec);
		default: return false;
		}
	}

	__forceinline bool hit(const ray& r, hitrec& rec, uint prim_id) const
	{
		switch (id) {
		case o_pol: return p.hit(r, rec, prim_id);
		case o_qua: return q.hit(r, rec, prim_id);
		case o_sph: return s.hit(r, rec, prim_id);
		case o_vox: return v.hit(r, rec, prim_id);
		default: return false;
		}
	}


	matrix get_trans()const {
		switch (id) {
		case o_pol: return p.get_trans();
		case o_qua: return q.get_trans();
		case o_sph: return s.get_trans();
		case o_vox: return v.get_trans();
		default: return matrix();
		}
	}
	void set_trans(const matrix& T) {
		switch (id) {
		case o_pol: p.set_trans(T); break;
		case o_qua: q.set_trans(T); break;
		case o_sph: s.set_trans(T); break;
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
	inline aabb get_box(uint prim_id)const {
		switch (id) {
		case o_pol: return p.get_box(prim_id);
		case o_qua: return q.get_box(prim_id);
		case o_sph: return s.get_box(prim_id);
		case o_vox: return v.get_box(prim_id);
		default: return aabb();
		}
	}
	__forceinline float pdf(const ray& r)const {
		if (!s.get_box().hit(r))return 0;
		switch (id) {
		case o_pol: return p.pdf(r);
		case o_qua: return q.pdf(r);
		case o_sph: return s.pdf(r);
		case o_vox: return v.pdf(r);
		default: return 0;
		}
	}
	__forceinline vec3 rand_to(vec3 O) const {
		switch (id) {
		case o_pol: return p.rand_to(O);
		case o_qua: return q.rand_to(O);
		case o_sph: return s.rand_to(O);
		case o_vox: return v.rand_to(O);


		default: return 0;
		}
	}
	__forceinline vec3 rand_from() const {
		switch (id) {
		case o_pol: return p.rand_from();
		case o_qua: return q.rand_from();
		case o_sph: return s.rand_from();
		case o_vox: return v.rand_from();
		default: return 0;
		}
	}
	union {
		mesh<poly> p;
		mesh<quad> q;
		mesh<sphere> s;
		mesh<voxel> v;
	};
	obj_enum id;
};

struct mesh_raw {
	mesh_raw(const poly* m, uint mat) : bbox(m->get_box()), p(m), mat(mat), id(o_pol) {}
	mesh_raw(const quad* m, uint mat) : bbox(m->get_box()), q(m), mat(mat), id(o_qua) {}
	mesh_raw(const sphere* m, uint mat) : bbox(m->get_box()), s(m), mat(mat), id(o_sph) {}
	mesh_raw(const voxel* m, uint mat) :bbox(m->get_box()), v(m), mat(mat), id(o_vox) {}
	__forceinline bool hit(const ray& r, hitrec& rec) const
	{
		if (!bbox.hit(r))return false;
		bool any_hit = 0;
		switch (id) {
		case o_pol: any_hit = p->hit(r, rec); break;
		case o_qua: any_hit = q->hit(r, rec); break;
		case o_sph: any_hit = s->hit(r, rec); break;
		case o_vox:	any_hit = v->hit(r, rec); break;
		default:break;
		}
		rec.mat = any_hit ? mat : rec.mat;
		return any_hit;
	}

	inline aabb get_box()const {
		return bbox;
	}
	inline void update_box() {
		switch (id) {
		case o_pol: bbox = p->get_box(); break;
		case o_qua: bbox = q->get_box(); break;
		case o_sph: bbox = s->get_box(); break;
		case o_vox: bbox = v->get_box(); break;
		default:break;
		}
	}
	aabb bbox;
	union {
		const poly* p;
		const quad* q;
		const sphere* s;
		const voxel* v;
	};
	uint mat;
	obj_enum id;
};
#pragma pack(pop)


