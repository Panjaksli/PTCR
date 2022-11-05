#pragma once
#include "texture.h"
#pragma pack(push,4)
class albedo {
public:
	albedo(const texture& _rgb = texture(vec3(0.5,0,0.5)), const texture& _mer = texture(vec3(0, 0, 1)), const texture& _nor = texture(vec3(0.5, 0.5, 1)), float _rep = 1.f) :
		_rgb(_rgb), _mer(_mer), _nor(_nor),rep(_rep) {}
	inline vec3 rgb(float u, float v)const {
		vec3 rgb = _rgb.sample(rep*u, rep*v);
#if GAMMA2
		return rgb * rgb;
#else 
		return rgb;
#endif
	}
	inline vec3 mer(float u, float v)const {
		return _mer.sample(rep*u, rep*v);
	}
	inline vec3 nor(float u, float v)const {
		return _nor.sample(rep*u, rep*v);
	}
	inline vec3 get_rgb()
	{
		return _rgb.get_col();
	}
	inline vec3 get_mer()
	{
		return _mer.get_col();
	}
	inline vec3 get_nor()
	{
		return _nor.get_col();
	}
	inline void set_rgb(vec3 col)
	{
		_rgb.set_col(col);
	}
	inline void set_mer(vec3 col)
	{
		_mer.set_col(col);
	}
	inline void set_nor(vec3 col)
	{
		_nor.set_col(col);
	}
private:
	texture _rgb, _mer, _nor;
	float rep;
};
#pragma pack(pop)