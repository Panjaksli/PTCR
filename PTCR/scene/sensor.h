#pragma once
#include "vec3.h"


class sensor {
public:
	sensor() {}
	sensor(uint _w, uint _h) : data(_w* _h),w(_w), h(_h), n(_w* _h) {}
	vector<vec3> data;
	uint* disp = nullptr;
	uint w = 0, h = 0, n = 0;
	uint pitch = 0;
	float spp = 0.f;


	__forceinline void add(uint i, uint j, const vec3& col) {
		uint off = i * w + j;
		data[off] += fixnan(col);
	}
	inline void set(uint i, uint j, const vec3& col) {
		uint off = i * w + j;
		data[off] = fixnan(col);
	}
	inline void clear(uint i) {
		data[i] = vec3();
	}
	inline void clear(uint i, uint j) {
		uint off = i * w + j;
		data[off] = vec3();
	}
	__forceinline void outpix(uint i, uint j) {
		uint off = i * w + j;
		uint off2 = i * pitch + j;
		bgr(data[off], disp[off2]);
	}
	inline void set_disp(uint* _disp, uint _pitch) {
		disp = _disp;
		pitch = _pitch;
	}
	inline void dt(float _t = 1.f) {
		spp += _t;
	}

	void clear();
	void resize(uint _w, uint _h);
	void outrgb();
};
