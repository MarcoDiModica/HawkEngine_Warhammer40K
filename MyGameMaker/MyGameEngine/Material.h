#pragma once
#include <string>

class Material
{
public:
	Material() = default;

	bool LoadTexture(const std::string& file_path);
	void Bind() const;

	unsigned int GetTextureID() const { return textureID; }

private:
	unsigned int textureID = 0;
	void CreateDefaultTexture();
};

