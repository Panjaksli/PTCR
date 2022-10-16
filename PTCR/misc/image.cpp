#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
image::image(const char* _filename) {

	auto filename = std::string(_filename);
	const auto dir = getenv("RTW_IMAGES");

	std::string cur = "", folder = "";
	if (dir && load(std::string(dir) + "/" + filename)) return;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) {
			if (j == 0)folder = "";
			else if (j == 1)folder = "textures/";
			else if (j == 2)folder = "textures/blocks/";
			if (load(cur + folder + filename)) return;
			if (load(cur + folder + filename + ".png")) return;
			if (load(cur + folder + filename + ".jpg")) return;
			if (load(cur + folder + filename + ".gif")) return;
			if (load(cur + folder + filename + ".tga")) return;
		}
		cur = "../" + cur;
	}
	std::cerr << "File not found!'" << filename << "'.\n";
}



bool image::load(const std::string filename) {
	int n = 4;
	int width, height;
	data = stbi_load(filename.c_str(), &width, &height, &n, 4);
	w = width, h = height;
	return data != nullptr;
}

image::~image() { STBI_FREE(data); }