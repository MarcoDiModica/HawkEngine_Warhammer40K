#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Shaders.h"
#include <yaml-cpp/yaml.h>

#include <string>

class UIImageComponent : public Component
{
public:
	explicit UIImageComponent(GameObject* owner);
	~UIImageComponent() override = default;

	UIImageComponent(const UIImageComponent&) = delete;
	UIImageComponent& operator=(const UIImageComponent&) = delete;

	UIImageComponent(UIImageComponent&&) noexcept = default;
	UIImageComponent& operator=(UIImageComponent&&) noexcept = default;

	void Awake() override;
	void Start() override;
	void Update(float deltaTime) override;
	void Destroy() override;

	std::unique_ptr<Component> Clone(GameObject* owner) override;

	ComponentType GetType() const override { return ComponentType::IMAGE; }
	const std::string& GetImagePath() const { return texturePath; }

	void SetTexture(std::string path);

	std::shared_ptr<Image> GetTexture() const { return texture; }

	void LoadMesh();

	void SetProjection(const glm::mat4& proj) { projection = proj; }

	glm::vec2 GetImageSize() const { return glm::vec2(texture->width(), texture->height()); }

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

	void SetColor(const glm::vec4& color) { this->color = color; }
	glm::vec4 GetColor() const { return color; }

	void SetSpriteSize(const glm::vec2& size) { spriteSize = size; }
	glm::vec2 GetSpriteSize() const { return spriteSize; }

	void SetAnimSpeed(float speed) { animSpeed = speed; }
	float GetAnimSpeed() const { return animSpeed; }

	void SetUseAnimation(bool use) { useAnimation = use; }
	bool GetUseAnimation() const { return useAnimation; }

	void SetAnimationNum(int num) { animationNum = num; }
	int GetAnimationNum() const { return animationNum; }

	void SetAnimationIndexLimit(int limit) { anim1IndexLimit = limit; }
	int GetAnimationIndexLimit() const { return anim1IndexLimit; }

private:
	//texture
	std::string texturePath;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	glm::vec4 color = glm::vec4(1.0f);
	Shaders * shader;
	glm::mat4 projection;

	bool useAnimation = false;
	glm::vec2 spriteSize = glm::vec2(0.0f, 0.0f);
	glm::vec2 sheetSize = glm::vec2(0.0f, 0.0f);
	glm::vec2 spriteOffset = glm::vec2(0.0f, 0.0f);
	float indexTimer = 0.0f;
	float animSpeed = 0.0f;
	int animIndex = 0;
	int anim1IndexLimit = 0;

	int animationNum = 0;

protected:

	friend class SceneSerializer;

    YAML::Node encode() override {  
       YAML::Node node = Component::encode();  

       node["texture_path"] = texture->image_path;  
       node["use_animation"] = useAnimation;  
       node["sprite_size"] = std::vector<float>{spriteSize.x, spriteSize.y};  
       node["sheet_size"] = std::vector<float>{sheetSize.x, sheetSize.y};  
       node["sprite_offset"] = std::vector<float>{spriteOffset.x, spriteOffset.y};  
       node["anim_speed"] = animSpeed;  

       /*node["shader"] = shader;*/  
       /*node["mesh"] = mesh;*/  

       return node;  
    }  

    bool decode(const YAML::Node& node) override {  
		if (!Component::decode(node)) {
			return false;
		}
		
       std::string path = node["texture_path"].as<std::string>();  
       useAnimation = node["use_animation"].as<bool>();  

       auto spriteSizeVec = node["sprite_size"].as<std::vector<float>>();  
       spriteSize = glm::vec2(spriteSizeVec[0], spriteSizeVec[1]);  

       auto sheetSizeVec = node["sheet_size"].as<std::vector<float>>();  
       sheetSize = glm::vec2(sheetSizeVec[0], sheetSizeVec[1]);  

       auto spriteOffsetVec = node["sprite_offset"].as<std::vector<float>>();  
       spriteOffset = glm::vec2(spriteOffsetVec[0], spriteOffsetVec[1]);  

       animSpeed = node["anim_speed"].as<float>();  
       SetTexture(path);  

       /*shader = node["shader"].as<Shaders*>();*/  
       /*mesh = node["mesh"].as<std::shared_ptr<Mesh>>();*/  

       return true;  
    }
};

