#include "Image.h"
#include <filesystem>
#include <vector>
#include <GL/glew.h>
#include "../MyGameEditor/Log.h"
#include <fstream>
#include <zlib.h>
#include <unordered_map>

namespace fs = std::filesystem;

#define CHECKERS_WIDTH 64
#define CHECKERS_HEIGHT 64

static auto formatFromChannels(int channels) {
	switch (channels) {
	case 1: return GL_LUMINANCE;
	case 2: return GL_LUMINANCE_ALPHA;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	default: return GL_RGB;
	}
}

Image::~Image() {
	if (_id) glDeleteTextures(1, &_id);
}

Image::Image(const Image& other) :
	_width(other._width),
	_height(other._height),
	_channels(other._channels),
	image_path(other.image_path) {

	if (other._id != 0) 
	{
		glGenTextures(1, &_id);

		GLint previousTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

		glBindTexture(GL_TEXTURE_2D, other._id);
		std::vector<unsigned char> pixels(_width * _height * _channels);
		glGetTexImage(GL_TEXTURE_2D, 0, formatFromChannels(_channels), GL_UNSIGNED_BYTE, pixels.data());

		glBindTexture(GL_TEXTURE_2D, _id);
		glTexImage2D(GL_TEXTURE_2D, 0, _channels, _width, _height, 0, formatFromChannels(_channels), GL_UNSIGNED_BYTE, pixels.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, previousTexture);
	}
}

Image& Image::operator=(const Image& other) {
	if (this != &other) {
		if (_id != 0) {
			glDeleteTextures(1, &_id);
		}

		_width = other._width;
		_height = other._height;
		_channels = other._channels;
		image_path = other.image_path;

		if (other._id != 0) {
			glGenTextures(1, &_id);

			GLint previousTexture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

			glBindTexture(GL_TEXTURE_2D, other._id);
			std::vector<unsigned char> pixels(_width * _height * _channels);
			glGetTexImage(GL_TEXTURE_2D, 0, formatFromChannels(_channels), GL_UNSIGNED_BYTE, pixels.data());

			glBindTexture(GL_TEXTURE_2D, _id);
			glTexImage2D(GL_TEXTURE_2D, 0, _channels, _width, _height, 0, formatFromChannels(_channels), GL_UNSIGNED_BYTE, pixels.data());
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, previousTexture);
		}
	}
	return *this;
}

Image& Image::operator=(Image&& other) noexcept {
	if (this != &other) {
		if (_id != 0) {
			glDeleteTextures(1, &_id);
		}

		_id = other._id;
		_width = other._width;
		_height = other._height;
		_channels = other._channels;
		image_path = std::move(other.image_path);

		other._id = 0;
	}
	return *this;
}

Image::Image(Image&& other) noexcept :
	_id(other._id),
	_width(other._width),
	_height(other._height),
	_channels(other._channels) {
	other._id = 0;
}

void Image::bind() const {
	glBindTexture(GL_TEXTURE_2D, _id);
}

static int rowAlignment(int width, int channels) {
	const size_t rowSizeInBytes = static_cast<size_t>(width) * channels;
	if ((rowSizeInBytes % 8) == 0) return 8;
	if ((rowSizeInBytes % 4) == 0) return 4;
	if ((rowSizeInBytes % 2) == 0) return 2;
	return 1;
}

void Image::load(int width, int height, int channels, void* data) {
	_width = width;
	_height = height;
	_channels = channels;

	if (!_id) glGenTextures(1, &_id);

	bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, rowAlignment(width, channels));
	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, formatFromChannels(channels), GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

struct ImageDTO {
	unsigned short width = 0;
	unsigned short height = 0;
	unsigned char channels = 0;
	std::vector<char> data;

	ImageDTO() = default;

	explicit ImageDTO(const Image& img) : width(img.width()), height(img.height()), channels(img.channels()), data(width* height* channels) {
		img.bind();
		glGetTexImage(GL_TEXTURE_2D, 0, formatFromChannels(img.channels()), GL_UNSIGNED_BYTE, data.data());
	}
};

std::ostream& operator<<(std::ostream& os, const Image& img) {
	ImageDTO dto(img);
	os.write((const char*)&dto.width, sizeof(dto.width));
	os.write((const char*)&dto.height, sizeof(dto.height));
	os.write((const char*)&dto.channels, sizeof(dto.channels));
	os.write(dto.data.data(), dto.data.size());
	return os;
}


std::istream& operator>>(std::istream& is, Image& img) {
	ImageDTO dto;
	is.read((char*)&dto.width, sizeof(dto.width));
	is.read((char*)&dto.height, sizeof(dto.height));
	is.read((char*)&dto.channels, sizeof(dto.channels));

	dto.data.resize(dto.width * dto.height * dto.channels);
	is.read(dto.data.data(), dto.data.size());

	img.load(dto.width, dto.height, dto.channels, dto.data.data());

	return is;
}

void Image::LoadTexture(const std::string& path)
{
	image_path = path;
	auto img = ilGenImage();
	ilBindImage(img);
	ilLoadImage(path.c_str());
	auto width = ilGetInteger(IL_IMAGE_WIDTH);

	auto height = ilGetInteger(IL_IMAGE_HEIGHT);

	auto channels = ilGetInteger(IL_IMAGE_CHANNELS);
	auto data = ilGetData();

	//load image as a texture in VRAM
	load(width, height, channels, data);

	//now we can delete image from RAM
	ilDeleteImage(img);
}

void Image::LoadTextureLocalPath(const std::string& path) {

	fs::path absPath = fs::absolute(path);
	image_path =  absPath.string();
	auto img = ilGenImage();
	ilBindImage(img);
	ilLoadImage(image_path.c_str());
	auto width = ilGetInteger(IL_IMAGE_WIDTH);

	auto height = ilGetInteger(IL_IMAGE_HEIGHT);

	auto channels = ilGetInteger(IL_IMAGE_CHANNELS);
	auto data = ilGetData();

	//load image as a texture in VRAM
	load(width, height, channels, data);

	//now we can delete image from RAM
	ilDeleteImage(img);

}

std::unordered_map<std::string, std::shared_ptr<Image>> imageCache;

void Image::SaveBinary(const std::string& filename) const {
	
	std::string fullPath = "Library/Images/" + filename + ".image";
	LOG(LogType::LOG_INFO, "Saving image to: %s", fullPath.c_str());

	if (!std::filesystem::exists("Library/Images")) {
		std::filesystem::create_directory("Library/Images");
	}

	std::ofstream fout(fullPath, std::ios::binary);
	if (!fout.is_open()) {
		return;
	}

	fout.write(reinterpret_cast<const char*>(&_width), sizeof(_width));
	fout.write(reinterpret_cast<const char*>(&_height), sizeof(_height));
	fout.write(reinterpret_cast<const char*>(&_channels), sizeof(_channels));

	ILubyte* pixelData = ilGetData(); //devil
	ILuint dataSize = _width * _height * _channels;

	fout.write(reinterpret_cast<const char*>(pixelData), dataSize);
}

std::shared_ptr<Image> Image::LoadBinary(const std::string& filename) {
	std::string fpath = filename.substr(0, filename.size() - 4);

	std::string fullPath = "Library/Images/" + fpath + ".image";
	LOG(LogType::LOG_INFO, "Loading image from: %s", fullPath.c_str());
	auto it = imageCache.find(fullPath);
	if (it != imageCache.end()) {
		return it->second;
	}

	std::shared_ptr<Image> img;

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		throw std::runtime_error("Error opening image file: " + fullPath);
	}

	img = std::make_shared<Image>();

	fin.read(reinterpret_cast<char*>(&img->_width), sizeof(img->_width));
	fin.read(reinterpret_cast<char*>(&img->_height), sizeof(img->_height));
	fin.read(reinterpret_cast<char*>(&img->_channels), sizeof(img->_channels));

	ILubyte* pixelData = new ILubyte[img->_width * img->_height * img->_channels];

	fin.read(reinterpret_cast<char*>(pixelData), img->_width * img->_height * img->_channels);

	ilTexImage(img->_width, img->_height, 1, img->_channels, IL_RGBA, IL_UNSIGNED_BYTE, pixelData);

	delete[] pixelData;

	imageCache[fullPath] = img;


	return img;
}