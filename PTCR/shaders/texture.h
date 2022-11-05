#pragma once
#include "vec3.h"
void save_png(void* data, uint w, uint h);
#pragma pack(push,2)
class texture {
public:
	virtual ~texture() = default;
	inline virtual tex_data sample(float u, float v)const = 0;//{ return tex_data(col(u, v), mer(u, v), nor(u, v)); }
private:
	inline vec3 col(float u, float v)const { return vec3(1, 0, 1); }
	inline vec3 mer(float u, float v)const { return vec3(0, 1, 0); }
	inline vec3 nor(float u, float v)const { return vec3(0.5, 0.5, 1); }
};
class uni_tex :public texture {
public:
	uni_tex(vec3 col = vec3(1, 1, 1, 1), vec3 mer = vec3(0, 0, 1), vec3 map = vec3(0.5, 0.5, 1)) :rgba(col), met(mer), map(map) {}
	inline tex_data sample(float u, float v)const override { return tex_data(col(u, v), mer(u, v), nor(u, v)); }
private:
	inline vec3 col(float u, float v)const {
#if GAMMA2
		return vec3(rgba * rgba, rgba.w);
#else
		return rgba;
#endif
	}
	inline vec3 mer(float u, float v)const { return met; }
	inline vec3 nor(float u, float v)const { return map; }

	vec3 rgba;
	vec3 met;
	vec3 map;
};

class ima_tex :public texture {
public:
	ima_tex(const char* col, float repeat = 1.f, vec3 mer = vec3(0, 0, 1), vec3 map = vec3(0.5, 0.5, 1)) :rgba(col), met(mer), map(map), repeat(repeat) {}
	inline tex_data sample(float u, float v)const override { return tex_data(col(u, v), mer(u, v), nor(u, v)); }
private:
	inline vec3 col(float u, float v)const {
		const uchar* p = rgba.pixel(u, v, repeat);
		vec3 aten(p[0], p[1], p[2], p[3]);
		aten *= 1.f / 255.f;
#if GAMMA2
		return vec3(aten * aten, aten.w);
#else
		return aten;
#endif
	}
	inline vec3 mer(float u, float v)const { return met; }
	inline vec3 nor(float u, float v)const { return map; }

	image rgba;
	vec3 met;
	vec3 map;
	float repeat;
};
class nor_tex :public texture {
public:
	nor_tex(vec3 col = vec3(0, 0, 0), vec3 mer = vec3(0, 0, 1), const char* map = nullptr, float repeat = 1.f) : map(map), met(mer), rgba(col), repeat(repeat) {}
	inline tex_data sample(float u, float v)const override { return tex_data(col(u, v), mer(u, v), nor(u, v)); }
private:
	inline vec3 col(float u, float v)const {
#if GAMMA2
		return vec3(rgba * rgba, rgba.w);
#else
		return rgba;
#endif
	}
	inline vec3 mer(float u, float v)const { return met; }
	inline vec3 nor(float u, float v)const {
		const uchar* p = map.pixel(u, v, repeat);
		vec3 aten(p[0], p[1], p[2], p[3]);
		aten *= 1.f / 255.f;
		return aten;
	}

	image map;
	vec3 met;
	vec3 rgba;
	float repeat;
};
class pbr_tex :public texture {
public:
	pbr_tex(const char* col, const char* mer, const char* map, float repeat = 1.f) :rgba(col), met(mer), map(map), repeat(repeat) {}
	inline tex_data sample(float u, float v)const override { return tex_data(col(u, v), mer(u, v), nor(u, v)); }
private:

	inline vec3 col(float u, float v)const {
		const uchar* p = rgba.pixel(u, v, repeat);
		vec3 aten(p[0], p[1], p[2], p[3]);
		aten *= 1.f / 255.f;
#if GAMMA2
		return vec3(aten * aten, aten.w);
#else
		return aten;
#endif
	}
	inline vec3 mer(float u, float v)const {
		const uchar* p = met.pixel(u, v, repeat);
		vec3 aten(p[0], p[1], p[2], p[3]);
		aten *= 1.f / 255.f;
		return aten;
	}
	inline vec3 nor(float u, float v)const {
		const uchar* p = map.pixel(u, v, repeat);
		vec3 aten(p[0], p[1], p[2], p[3]);
		aten *= 1.f / 255.f;
		return aten;
	}
	inline void set_col(vec3 _rgb) {
		if (solid)rgb = _rgb;
	}
	inline vec3 get_col()
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

