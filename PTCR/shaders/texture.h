#pragma once
#include <cstdlib>
#include <iostream>
#include "vec3.h"
void save_png(void* data, uint w, uint h);
void save_hdr(vector<vec3>& data, uint w, uint h, int spp = 0);
#pragma pack(push,2)
class texture {
public:

	texture() : rgb(0.5, 0.5, 1, 1), solid(1) {}
	texture(vec3 _rgb) :rgb(_rgb), solid(1) {}
	texture(const char* _filename) : solid(0) {
		if (_filename == nullptr)
		{
			solid = 1;
			rgb = 0;
		}
		else {
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
	}
	texture(const texture& cpy):solid(cpy.solid) {
		if (solid) {
			rgb = cpy.rgb;
		}
		else {
			w = cpy.w, h = cpy.h;
			data = (uchar*)malloc(w * h * 4);
			memcpy(data, cpy.data, w * h * 4);
		}
	}
	const texture & operator=(const texture& cpy){
		solid = cpy.solid;
		if (solid) {
			rgb = cpy.rgb;
		}
		else {
			w = cpy.w, h = cpy.h;
			data = (uchar*)malloc(w * h * 4);
			memcpy(data, cpy.data, w * h * 4);
		}
		return *this;
	}
	~texture() {
		clear();
	}
	__forceinline vec3 sample(float u, float v) const {
		if (solid)return rgb;
		uint x = u * (w - 1);
		uint y = v * (h - 1);
		x %= w;
		y %= h;
		uchar r = data[(x + y * w) * 4];
		uchar g = data[(x + y * w) * 4 + 1];
		uchar b = data[(x + y * w) * 4 + 2];
		uchar a = data[(x + y * w) * 4 + 3];
		return vec3(r, g, b, a) * (1 / 255.f);
	}
	void set_col(vec3 _rgb) {
		if (solid)rgb = _rgb;
	}
	vec3 get_col()
	{
		return solid*rgb;
	}
private:
	union {
		struct {
			uchar* data;
			uint w, h;
		};
		vec3 rgb;
	};
	bool solid;
	void clear() { if (!solid) free(data); }
	bool load(const std::string filename);
	
};
#pragma pack(pop)
