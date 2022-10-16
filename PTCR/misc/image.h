#pragma once
#include <cstdlib>
#include <iostream>
#include "defines.h"
class image {
public:
	image() : data(nullptr) {}
	image(const char* _filename);
	~image();
	bool load(const std::string filename);

	inline const uchar* pixel(float u, float v, float repeat = 1.f) const {
		const static uchar undef[] = { 127,127,255 };
		if (data == nullptr) return undef;

		uint x = u * repeat * (w - 1);
		uint y = v * repeat * (h - 1);
		x %= w;
		y %= h;
		return &data[(x + y * w) * 4];
	}

private:
	uchar* data;
	uint w, h;
};
