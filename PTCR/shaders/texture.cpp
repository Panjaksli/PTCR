#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "texture.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <ctime>
#include <string>
bool texture::load(const std::string filename) {
	int n = 4;
	int width, height;
	data = stbi_load(filename.c_str(), &width, &height, &n, 4);
	w = width;
	h = height;
	return data != nullptr;
}

void save_hdr(vector<vec3>& img, uint w, uint h, int spp) {
	static char name[20];
	time_t now = time(0);
	std::tm tm;
	localtime_s(&tm, &now);
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", &tm);
	std::string file(name);
	file = "screenshots/" + file + "_" + std::to_string(spp) + "SPP.png";
	vector<uint> buff((size_t)w * h);

	for (uint i = 0; i < img.size(); i++)
		rgb(img[i], buff[i]);
	if (stbi_write_png(file.c_str(), w, h, 4, &buff[0], 4 * w))
		cout << "Saved file in: " << file << "\n";
	else throw "Save failed !!!\n";

	vector<vec3> tmp(w * h);
#pragma omp parallel for collapse(2) schedule(static,64)
	for (int i = 1; i < h - 1; i++) {
		for (int j = 1; j < w - 1; j++) {
			vec3 window[9];
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++)
					window[k * 3 + l] = img[(i + k - 1) * w + j + l - 1];
			tmp[i * w + j] = med9(window);
		}
	}
	for (uint i = 0; i < tmp.size(); i++)
		rgb(tmp[i], buff[i]);
	file = "screenshots/" + std::string(name) + "_" + std::to_string(spp) + "SPP_den.png";
	if (stbi_write_png(file.c_str(), w, h, 4, &buff[0], 4 * w))
		cout << "Saved file in: " << file << "\n";
	else throw "Save failed !!!\n";
		
}