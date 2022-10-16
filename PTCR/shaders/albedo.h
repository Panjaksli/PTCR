#pragma once
#include "texture.h"
class albedo {
public:
	albedo(const texture& _rgb = texture(vec3(0.5,0,0.5)), const texture& _mer = texture(vec3(0, 0, 1)), const texture& _nor = texture(vec3(0.5, 0.5, 1))) :_rgb(_rgb), _mer(_mer), _nor(_nor) {}
	inline vec3 rgb(float u, float v)const {
		vec3 rgb = _rgb.sample(u, v);
#if GAMMA2
		return rgb * rgb;
#else 
		return rgb;
#endif
	}
	inline vec3 mer(float u, float v)const {
		return _mer.sample(u, v);
	}
	inline vec3 nor(float u, float v)const {
		return _nor.sample(u, v);
	}
	void clean() {
		_rgb.clean();
		_mer.clean();
		_nor.clean();
	}
private:
	texture _rgb, _mer, _nor;
};