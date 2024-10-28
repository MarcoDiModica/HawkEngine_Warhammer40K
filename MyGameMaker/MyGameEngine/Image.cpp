#include "Image.h"
#include <vector>
#include <GL/glew.h>

Image::~Image() {
	if (_id) glDeleteTextures(1, &_id);
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

static auto formatFromChannels(int channels) {
	switch (channels) {
	case 1: return GL_LUMINANCE;
	case 2: return GL_LUMINANCE_ALPHA;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	default: return GL_RGB;
	}
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
	auto img = ilGenImage();
	ilBindImage(img);
	ilLoadImage((const wchar_t*)path.c_str());
	auto width = ilGetInteger(IL_IMAGE_WIDTH);

	auto height = ilGetInteger(IL_IMAGE_HEIGHT);

	auto channels = ilGetInteger(IL_IMAGE_CHANNELS);
	auto data = ilGetData();

	//load image as a texture in VRAM
	load(width, height, channels, data);



	//now we can delete image from RAM
	ilDeleteImage(img);
}