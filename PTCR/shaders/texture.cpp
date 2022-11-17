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

void save_png(void* img, uint w, uint h) {
	static char name[20];
	time_t now = time(0);
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", localtime(&now));
	std::string file(name);
	file = "screenshots/" + file + ".png";
	uint* buff = new uint[w * h];
	for (int i = 0; i < w * h; i++) {
		uint bgra = ((uint*)img)[i];
		uint b = (bgra & 0xff) << 16;
		uint g = (bgra & 0xff00);
		uint r = (bgra & 0xff0000) >> 16;
		uint a = (bgra & 0xff000000);
		buff[i] = b + g + r + a;
	}
	if (stbi_write_png(file.c_str(), w, h, 4, buff, 4 * w))
	{
		cout << "Saved file in: " << file << "\n";
	}
	else {
		cout << "Save failed !!!\n";
	}
	delete[]buff;
}
void save_hdr(vector<vec3>& img, uint w, uint h, int spp) {
	static char name[20];
	time_t now = time(0);
	std::tm tm;
	localtime_s(&tm, &now);
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", &tm);
	std::string file(name);
	file = "screenshots/" + file + "_" + std::to_string(spp) + "SPP.png";
	uint* buff = new uint[w * h];
	for (uint i = 0; i < img.size(); i++)
	{
		rgb(img[i], buff[i]);
	}
	if (stbi_write_png(file.c_str(), w, h, 4, buff, 4 * w))
	{
		cout << "Saved file in: " << file << "\n";
	}
	else {
		cout << "Save failed !!!\n";
	}
	delete[]buff;
}