#pragma once
#include <cstdlib>
#include <iostream>
#include "vec3.h"
#pragma pack(push,4)
class texture {
public:

	texture() : rgb(0.5, 0.5, 1, 1), rep(1.f), solid(1) {}
	texture(vec3 _rgb, float _rep = 1.f) :rgb(_rgb), rep(_rep), solid(1) {}
	texture(const char* _filename, float _rep = 1.f) :rep(_rep), solid(0) {
		std::string filename = std::string(_filename);
		bool found = load(filename) || load(filename + ".jpg") || load(filename + ".png") || load(filename + ".gif") || load(filename + ".tga")
			|| load("textures/" + filename) || load("textures/" + filename + ".jpg") || load("textures/" + filename + ".png") ||
			load("textures/" + filename + ".gif") || load("textures/" + filename + ".tga");
		if (!found) {
			solid = 1;
			rgb = vec3(0.5, 0.5, 1, 1);
			std::cerr << "File not found!'" << filename << "'.\n";
		}
	}
	inline vec3 sample(float u, float v) const {
		if (solid)return rgb;
		uint x = u * rep * (w - 1);
		uint y = v * rep * (h - 1);
		x %= w;
		y %= h;
		uchar r = data[(x + y * w) * 4];
		uchar g = data[(x + y * w) * 4 + 1];
		uchar b = data[(x + y * w) * 4 + 2];
		uchar a = data[(x + y * w) * 4 + 3];
		return vec3(r, g, b, a) * (1 / 255.f);
	}
	void clean() { if (!solid) free(data); }
private:
	bool load(const std::string filename);
	union {
		struct {
			uchar* data;
			uint w, h;
		};
		vec3 rgb;
	};
	float rep;
	bool solid;
};
#pragma pack(pop)