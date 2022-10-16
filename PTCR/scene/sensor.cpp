#include <omp.h>
#include "sensor.h"
void sensor::clear() {
#pragma omp parallel for schedule(static,100)
	for (auto& buff : data)
		buff = vec3();
	t = 0.f;
}
void sensor::resize(uint _w, uint _h) {
	w = _w;
	h = _h;
	n = w * h;
	data.resize(n);
	disp.resize(n);
}
void sensor::present() {
#pragma omp parallel for schedule(static,100) collapse(2)
	for (int i = 1; i < h - 1; i++)
	{
		for (int j = 1; j < w - 1; j++) {
			vec3 col[9];
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++)
					col[k * 3 + l] = data[(i + k - 1) * w + j + l - 1];
			vec3 out = med9(col);
			rgb_avg(out, disp[i * w + j]);
		}
	}
}
void sensor::outrgb() {

}