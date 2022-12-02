#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"
#include "stb/stb_image.h"
#include <string>
bool texture::load(const std::string filename) {
	int n = 4;
	int width, height;
	data = stbi_load(filename.c_str(), &width, &height, &n, 4);
	w = width;
	h = height;
	return data != nullptr;
}

