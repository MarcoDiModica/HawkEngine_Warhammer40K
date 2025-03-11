#pragma once

#include <ostream>
#include <istream>
#include <memory>
#include <IL/il.h>
#include <IL/ilu.h>
#include <glm/glm.hpp>

class Image{

	unsigned int _id = 0;
	unsigned short _width = 0;
	unsigned short _height = 0;
	unsigned char _channels = 0;

public:
	unsigned int id() const { return _id; }
	auto width() const { return _width; }
	auto height() const { return _height; }
	auto channels() const { return _channels; }

	Image() = default;
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept = delete;
	~Image();

	void bind(unsigned int slot = 0) const;
	void load(int width, int height, int channels, void* data);
	// Load Texture
	bool LoadTexture(const std::string& path);

	void LoadTextureLocalPath(const std::string& path);

	void SaveBinary(const std::string& path) const;
	static std::shared_ptr<Image> LoadBinary(const std::string& path);

	std::string image_path;
};

std::ostream& operator<<(std::ostream& os, const Image& img);
std::istream& operator>>(std::istream& is, Image& img);