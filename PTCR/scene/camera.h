#pragma once
#include "sensor.h"
#include "matrix.h"
#include "ray.h"
#include "obj.h"
class camera
{
public:
	camera() {}
	camera(float fov, uint _w, uint _h, matrix _T = matrix()) :T(_T), CCD(_w, _h), fov(fov), tfov(tan(0.5f * torad(fov))), iw(1.0 / _w), ih(1.0 / _h), asp((float)_w / _h), speed(0.01) {}
	matrix T = matrix();
	sensor CCD = sensor(1280, 720);
	uint& w = CCD.w, & h = CCD.h;
	float fov = 90, tfov = tan(0.5f * torad(fov));
	float iw = 1.f / w, ih = 1.f / h, asp = float(w) / h;
	float speed = 0.005f;
	float exposure = 1.f;
	float fstop = 16.f;
	float foc_l = 0.0216f;
	float foc_t = 1.f;
	const float frame = 0.035;
	const float diagonal = 0.0432;
	bool autofocus = 1;
	bool moving = 1;


	__forceinline ray raycast(float py, float px) const
	{
		float SSX = 2.f * (px + 0.5f) * iw - 1.f;
		float SSY = 1.f - 2.f * (py + 0.5f) * ih;
		px = SSX * tfov * asp;
		py = SSY * tfov;
		vec3 D(px, py, -1);
		D = foc_t * (T * D);
		vec3 r = foc_l * sa_disk() / fstop;
		r = T * r;
		vec3 O(T.P());
		return ray(O - r, D + r, 1);
	}
	__forceinline void pixel(uint y, uint x, vec3 col)
	{
		CCD.add(y, x, col * exposure);
		CCD.outpix(y, x);
	}

	void update();
	void set_P(vec3 P);
	void move_free(float frw, float up, float sid);
	void move_fps(float frw, float up, float sid);
	void rotate(float alfa, float beta, float gamma);
	void resize(uint _w, uint _h, float scale);
	void setup(matrix _T, float _fov, float _fstop = 16);
	void set_fov(float _fov);
	ray focus_ray(float py = 0.5f, float px = 0.5f) const;

	inline float m_to_fov(float m) {
		return 2.f * atanf(diagonal / (2.f * m));
	}
	inline float fov_to_m(float fov) {

		return diagonal / (2 * tanf(0.5f * torad(fov)));
	}
	
};

