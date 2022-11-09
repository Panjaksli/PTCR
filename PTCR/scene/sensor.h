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
	float t = 0.f;


	__forceinline void add(uint col, uint row, const vec3& t) {
		uint off = col * w + row;
		data[off] += fixnan(vec3(t, 1));
	}
	inline void set(uint col, uint row, const vec3& t) {
		uint off = col * w + row;
		data[off] = fixnan(vec3(t, 1));
	}
	inline void clear(uint i) {
		data[i] = vec3();
	}
	inline void clear(uint col, uint row) {
		uint off = col * w + row;
		data[off] = vec3();
	}
	__forceinline void outpix(uint col, uint row) {
		uint off = col * w + row;
		uint off2 = col * pitch + row;
		bgr(data[off], disp[off2]);
	}
	inline void set_disp(uint* _disp, uint _pitch) {
		disp = _disp;
		pitch = _pitch;
	}
	inline void dt(float _t = 1.f) {
		t += _t;
	}

	void clear();
	void resize(uint _w, uint _h);
	void outrgb();
};
