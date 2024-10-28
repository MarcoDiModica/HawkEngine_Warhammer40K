#pragma once

#include <memory>
#include "Image.h"

class Material
{
public:
	enum WrapModes { Repeat, MirroredRepeat, Clamp };
	WrapModes wrapMode = Repeat;

	enum Filters { Nearest, Linear };
	Filters filter = Nearest;

private:
	std::shared_ptr<Image> imagePtr;

public:

	void LoadTexture(const std::string& filename);
	unsigned int id() const { return imagePtr ? imagePtr->id() : 0; }
	void bind() const;
	void setImage(const std::shared_ptr<Image>& img_ptr) { imagePtr = img_ptr; }
	const auto& image() const { return *imagePtr; }
};

