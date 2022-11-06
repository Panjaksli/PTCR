#pragma once
#include "vec3.h"


class sensor {
public:
	sensor() {}
	sensor(uint _w, uint _h) : data(_w* _h), disp(_w* _h), w(_w), h(_h), n(_w* _h) {}

	inline void add(uint col, uint row, const vec3& t) {
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
	inline void outpix(uint col, uint row) {
		uint off = col * w + row;
		rgb(data[off], disp[off]);//rgb_avg(data[off], disp[off]);
	}
	inline void dt(float _t = 1.f) {
		t += _t;
	}

	void clear();
	void resize(uint _w, uint _h);
	void present();
	void outrgb();

	vector<vec3> data;
	vector<uint> disp;
	uint w = 0, h = 0, n = 0;
	float exposure = 1.f;
	float t = 0.f;

};
