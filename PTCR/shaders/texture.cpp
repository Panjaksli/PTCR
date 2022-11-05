#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "texture.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <ctime>
bool texture::load(const std::string filename) {
	int n = 4;
	int width, height;
	data = stbi_load(filename.c_str(), &width, &height, &n, 4);
	w = width, h = height;
	return data != nullptr;
}

void save_png(void* data, uint w, uint h) {

	static char name[20];
	time_t now = time(0);
	strftime(name, sizeof(name), "%Y%m%d_%H%M%S", localtime(&now));
	std::string file(name);
	file = "screenshots/" + file + ".png";
	if (stbi_write_png(file.c_str(), w, h, 4, data, 4 * w))
	{
		cout << "Saved file in: " << file << "\n";
	}
	else {
		cout << "Save failed !!!\n";
	}
}
