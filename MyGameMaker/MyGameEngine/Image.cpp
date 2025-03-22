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

void Image::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
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

	if (!data || width <= 0 || height <= 0) {
		LOG(LogType::LOG_ERROR, "Datos de imagen inválidos: ancho=%d, alto=%d, canales=%d",
			width, height, channels);
		return;
	}

	GLint internalFormat;
	GLenum format;

	switch (channels) {
	case 1:
		internalFormat = GL_R8;
		format = GL_RED;
		break;
	case 2:
		internalFormat = GL_RG8;
		format = GL_RG;
		break;
	case 3:
		internalFormat = GL_RGB8;
		format = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
		break;
	default:
		LOG(LogType::LOG_WARNING, "Número de canales inesperado: %d, usando RGB", channels);
		internalFormat = GL_RGB8;
		format = GL_RGB;
		break;
	}

	if (!_id) {
		glGenTextures(1, &_id);
	}

	bind();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
		format, GL_UNSIGNED_BYTE, data);

	GLenum glError = glGetError();
	if (glError != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "Error de OpenGL al cargar textura: 0x%x", glError);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	swizzling:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

	glBindTexture(GL_TEXTURE_2D, 0);
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

bool Image::LoadTexture(const std::string& path) {
	LOG(LogType::LOG_INFO, "loading tex: %s", path.c_str());
	image_path = path;

	if (!std::filesystem::exists(path)) {
		LOG(LogType::LOG_ERROR, "tex not found at: %s", path.c_str());
		return false;
	}

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	ILuint img = ilGenImage();
	ilBindImage(img);

	ILboolean success = ilLoadImage(path.c_str());

	if (!success) {
		ILenum error = ilGetError();
		LOG(LogType::LOG_ERROR, "error loading image %s: %s (code: %d)",
			path.c_str(), iluErrorString(error), error);
		ilDeleteImage(img);
		return false;
	}

	auto format = ilGetInteger(IL_IMAGE_FORMAT);
	auto bpp = ilGetInteger(IL_IMAGE_BPP);

	LOG(LogType::LOG_INFO, "og image: BPP=%d, format=%d", bpp, format);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	if (ilGetInteger(IL_IMAGE_ORIGIN) != IL_ORIGIN_UPPER_LEFT) {
		iluFlipImage();
	}

	auto width = ilGetInteger(IL_IMAGE_WIDTH);
	auto height = ilGetInteger(IL_IMAGE_HEIGHT);
	auto channels = ilGetInteger(IL_IMAGE_BPP);
	auto data = ilGetData();

	LOG(LogType::LOG_INFO, "converted image: w=%d, h=%d, ch=%d, f=%d",
		width, height, channels, ilGetInteger(IL_IMAGE_FORMAT));

	load(width, height, channels, data);

	std::filesystem::path filePath(path);
	std::string fileName = filePath.stem().string();
	image_name = fileName;

	ilDeleteImage(img);

	return (_id != 0);
}

void Image::LoadTextureLocalPath(const std::string& path) {

	fs::path absPath = fs::absolute(path);
	image_path =  absPath.string();
	auto img = ilGenImage();
	ilBindImage(img);
	ilLoadImage(image_path.c_str());
	const auto origin = ilGetInteger(IL_IMAGE_ORIGIN);
	if (origin != IL_ORIGIN_UPPER_LEFT) iluFlipImage();
	auto width = ilGetInteger(IL_IMAGE_WIDTH);

	auto height = ilGetInteger(IL_IMAGE_HEIGHT);

	auto channels = ilGetInteger(IL_IMAGE_BPP);
	auto data = ilGetData();

	load(width, height, channels, data);

	ilDeleteImage(img);

}

std::unordered_map<std::string, std::shared_ptr<Image>> imageCache;

void Image::SaveBinary(const std::string& filename) const {
	if (!std::filesystem::exists("Library/Images")) {
		std::filesystem::create_directories("Library/Images");
	}

	std::string fullPath = "Library/Images/" + filename + ".image";
	LOG(LogType::LOG_INFO, "Saving image to: %s", fullPath.c_str());

	std::ofstream fout(fullPath, std::ios::binary);
	if (!fout.is_open()) {
		LOG(LogType::LOG_ERROR, "Failed to open file for writing: %s", fullPath.c_str());
		return;
	}

	fout.write(reinterpret_cast<const char*>(&_width), sizeof(_width));
	fout.write(reinterpret_cast<const char*>(&_height), sizeof(_height));
	fout.write(reinterpret_cast<const char*>(&_channels), sizeof(_channels));

	size_t pathLen = image_path.length();
	fout.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
	fout.write(image_path.c_str(), pathLen);

	std::vector<unsigned char> pixels(_width * _height * _channels);

	glBindTexture(GL_TEXTURE_2D, _id);

	GLenum format = formatFromChannels(_channels);

	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, pixels.data());

	glBindTexture(GL_TEXTURE_2D, 0);

	fout.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());

	if (!fout.good()) {
		LOG(LogType::LOG_ERROR, "Error writing image data to file: %s", fullPath.c_str());
	}

	fout.close();
	LOG(LogType::LOG_INFO, "Successfully saved image to: %s", fullPath.c_str());
}

std::shared_ptr<Image> Image::LoadBinary(const std::string& filename) {
	std::string baseFilename = filename;
	if (baseFilename.size() > 6 &&
		baseFilename.substr(baseFilename.size() - 6) == ".image") {
		baseFilename = baseFilename.substr(0, baseFilename.size() - 6);
	}

	std::string fullPath = "Library/Images/" + baseFilename + ".image";
	LOG(LogType::LOG_INFO, "Loading image from: %s", fullPath.c_str());

	auto it = imageCache.find(fullPath);
	if (it != imageCache.end()) {
		LOG(LogType::LOG_INFO, "Found image in cache: %s", fullPath.c_str());
		return it->second;
	}

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		LOG(LogType::LOG_ERROR, "Failed to open file for reading: %s", fullPath.c_str());
		return nullptr;
	}

	auto img = std::make_shared<Image>();

	fin.read(reinterpret_cast<char*>(&img->_width), sizeof(img->_width));
	fin.read(reinterpret_cast<char*>(&img->_height), sizeof(img->_height));
	fin.read(reinterpret_cast<char*>(&img->_channels), sizeof(img->_channels));

	size_t pathLen;
	fin.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
	img->image_path.resize(pathLen);
	fin.read(&img->image_path[0], pathLen);

	std::vector<unsigned char> pixels(img->_width * img->_height * img->_channels);
	fin.read(reinterpret_cast<char*>(pixels.data()), pixels.size());

	if (!fin.good() && !fin.eof()) {
		LOG(LogType::LOG_ERROR, "Error reading image data from file: %s", fullPath.c_str());
		return nullptr;
	}

	fin.close();

	glGenTextures(1, &img->_id);
	glBindTexture(GL_TEXTURE_2D, img->_id);

	GLint internalFormat;
	GLenum format;

	switch (img->_channels) {
	case 1:
		internalFormat = GL_R8;
		format = GL_RED;
		break;
	case 2:
		internalFormat = GL_RG8;
		format = GL_RG;
		break;
	case 3:
		internalFormat = GL_RGB8;
		format = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
		break;
	default:
		LOG(LogType::LOG_WARNING, "Unexpected number of channels: %d, using RGB", img->_channels);
		internalFormat = GL_RGB8;
		format = GL_RGB;
		break;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, img->_width, img->_height, 0,
		format, GL_UNSIGNED_BYTE, pixels.data());

	GLenum glError = glGetError();
	if (glError != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "OpenGL error when loading texture: 0x%x", glError);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

	glBindTexture(GL_TEXTURE_2D, 0);

	imageCache[fullPath] = img;

	LOG(LogType::LOG_INFO, "Successfully loaded image: %s (%dx%d, %d channels)",
		fullPath.c_str(), img->_width, img->_height, img->_channels);

	return img;
}