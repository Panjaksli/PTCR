#include "camera.h"
void camera::update()
{
	fov = fminf(179, fmaxf(1, fov));
	tfov = tan(0.5f * torad(fov));
	foc_l = fov_to_m(fov);
}

void camera::set_P(vec3 P)
{
	T.set_P(P);
	moving = 1;
}
void camera::move_free(float frw, float up, float sid)
{
	vec3 u = T * vec3(1, 0, 0);
	vec3 d = T * vec3(0, 0, -1);
	vec3 si(u.x(), 0, u.z());
	si = norm(si);
	vec3 P = frw * d * speed + sid * si * speed;
	P.xyz[1] += up * speed;
	T.add_P(P);
	moving = 1;
}
void camera::move_fps(float frw, float up, float sid)
{
	vec3 d = T * vec3(0, 0, -1);
	vec3 u = T * vec3(1, 0, 0);
	vec3 fw(d.x(), 0, d.z()), si(u.x(), 0, u.z());
	fw = norm(fw);
	si = norm(si);
	vec3 P = frw * fw * speed + sid * si * speed;
	P.xyz[1] += up * speed;
	T.add_P(P);
	moving = 1;
}
void camera::rotate(float alfa, float beta, float gamma)
{
	T.add_A(fov * (1.f / 90.f) * vec3(alfa, beta, gamma));
	moving = 1;
}
void camera::resize(uint _w, uint _h, float scale)
{
	w = _w * scale;
	h = _h * scale;
	iw = 1.0 / w;
	ih = 1.0 / h;
	asp = (float)w / h;
	CCD.resize(w, h);
	moving = 1;
}
void camera::reset_opt() {
	fov = 90, tfov = tan(0.5f * torad(fov));
	iw = 1.f / w, ih = 1.f / h, asp = float(w) / h;
	speed = 0.02f;
	exposure = 1.f;
	fstop = 16.f;
	foc_l = 0.0216f;
	foc_t = 1.f;
	autofocus = 1;
	moving = 1;
}
void camera::setup(matrix _T, float _fov, float _fstop) {
	T = _T;
	fov = _fov;
	fstop = _fstop;
	update();
}
void camera::set_fov(float _fov) {
	fov = _fov;
	update();
}
ray camera::focus_ray(float py, float px) const
{
	py *= h; px *= w;
	SS(py, px);
	vec3 D(px, py, -1);
	D = (T * D);
	vec3 O(T.P());
	return ray(O, D, 1);
}