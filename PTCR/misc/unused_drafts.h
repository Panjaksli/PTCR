#pragma once
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
		ray tr(r.O - P, r.D, r.iD);
		bool any_hit = false;
		for (uint i = 0; i < size; i++)
			any_hit |= prim[i].hit(tr, rec);
		rec.mat = any_hit ? mat : rec.mat;
		rec.P = any_hit ? rec.P + P : rec.P;
		return any_hit;
	}

	__forceinline bool hit(const ray& r, hitrec& rec, uint prim_id) const
	{
		ray tr(r.O - P, r.D, r.iD);
		bool any_hit = prim[prim_id].hit(tr, rec);
		rec.mat = any_hit ? mat : rec.mat;
		rec.P = any_hit ? rec.P + P : rec.P;
		return any_hit;
	}

	__forceinline float pdf(const ray& r)const {
		ray tr(r.O - P, r.D, r.iD);
		if (size == 1) return prim[0].trans(P).pdf(r);
		float y = 0.f;
		for (uint i = 0; i < size; i++)
		{
			y += lw * prim[i].trans(P).pdf(r);
		}
		return y;
	}
	__forceinline vec3 rand_to(vec3 O) const {
		if (size == 1) return prim[0].trans(P).rand_to(O);
		uint id = raint(size - 1);
		return prim[id].trans(P).rand_to(O);
	}
	__forceinline vec3 rand_from() const {
		if (size == 1) return prim[0].trans(P).rand_from();
		uint id = raint(size - 1);
		return prim[id].trans(P).rand_from();
	}

	inline aabb get_box()const { return bbox; }
	inline aabb get_box(uint prim_id)const { return prim[prim_id].trans(P).get_box(); }
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
		//matrix dT1(-P);
		matrix dT2(0, _T.A - A);
		//matrix dT3(_T.P(), 0);
		P = _T.P();
		A = _T.A;
#pragma omp parallel for schedule(static,64)
		for (uint i = 0; i < size; i++)
		{
			//prim[i] = prim[i].trans(dT1);
			prim[i] = prim[i].trans(dT2);
			//prim[i] = prim[i].trans(dT3);
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
		{
			bbox.join(get_box(i));
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


struct mesh_raw {
	mesh_raw(const mesh_var* obj, uint prim_id) : bbox(obj->get_box(prim_id)), obj(obj), prim_id(prim_id) {}

	__forceinline bool hit(const ray& r, hitrec& rec) const
	{
		if (!bbox.hit(r))return false;
		return obj->hit(r, rec, prim_id);
	}

	inline aabb get_box()const {
		return bbox;
	}
	inline void update_box() {
		bbox = obj->get_box(prim_id);
	}
	aabb bbox;
	const mesh_var* obj;
	uint prim_id;
};

//obj_bvh.emplace_back(mesh_raw(&obj, j));


void median_filter(const vector<vec3>& in, vector<vec3>& out, int h, int w)
{
#pragma omp parallel for collapse(2) schedule(dynamic,100)
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			vec3 window[9];
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++)
				{
					int a = i + k - 1;
					int b = j + l - 1;
					a = (a < 0 || a >= h) ? i : a;
					b = (b < 0 || b >= w) ? j : b;
					window[k * 3 + l] = in[a * w + b];
				}
			out[i * w + j] = med9(window);
		}
	}
}