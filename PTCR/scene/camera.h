#pragma once
#include "sensor.h"
#include "matrix.h"
#include "ray.h"
#include "obj.h"
class camera
{
public:
	camera(){}
	camera(float fov, uint _w, uint _h, matrix _T = matrix()) :T(_T), CCD(_w, _h), fov(fov), tfov(tan(0.5f * torad(fov))), iw(1.0 / _w), ih(1.0 / _h), asp((float)_w / _h), speed(0.01) {}

	inline ray raycast(float py, float px) const
	{
		vec3 u = T * vec3(1, 0, 0);
		vec3 v = T * vec3(0, 1, 0);
		float SSX = 2.f * (px + 0.5f) * iw - 1.f;
		float SSY = 1.f - 2.f * (py + 0.5f) * ih;
		px = SSX * tfov * asp;
		py = SSY * tfov;
		vec3 D(px, py, -1);
		D = foc_t * (T * D);
		float r[2]; rafl_tuple(r);
		r[0] *= (0.01f / fstop);
		r[1] *= (0.01f / fstop);
		vec3 off = u * r[0] + v * r[1];
		vec3 O(T.P());
		return ray(O + off, D - off, 1);
	}
	inline void pixel(uint y, uint x, vec3 col)
	{
		CCD.add(y, x, col);
		CCD.outpix(y, x);
	}
	

	void update();
	void set_P(vec3 P);
	void move_free(float frw, float up, float sid);
	void move_fps(float frw, float up, float sid);
	void rotate(float alfa, float beta, float gamma);
	void resize(uint _w, uint _h, float scale);
	ray focus_ray() const;


	matrix T = matrix();
	sensor CCD = sensor(1280, 720);
	uint& w = CCD.w, & h = CCD.h;
	float fov = 90, tfov = tan(0.5f * torad(fov));
	float iw = 1.f / w, ih = 1.f / h, asp = float(w) / h;
	float speed = 0.005f;
	float foc_t = 1.f;
	float fstop = 16.f;
	bool autofocus = 1;
	bool moving = 1;
};

