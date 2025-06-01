#pragma once

#include "../MyGameEngine/Component.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/Shaders.h"
#include <yaml-cpp/yaml.h>

#include <string>
#include "MyGameEngine/Material.h"
#include "MyGameEditor/BindlessManager.h"

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

	void SetSpriteSize(const glm::vec2& size) { material->spriteSize = size; }
	glm::vec2 GetSpriteSize() const { return material->spriteSize; }

	void SetAnimSpeed(float speed) { animSpeed = speed; }
	float GetAnimSpeed() const { return animSpeed; }

	void SetUseAnimation(bool use) { useAnimation = use; }
	bool GetUseAnimation() const { return useAnimation; }

	void SetAnimationNum(int num) { animationNum = num; }
	int GetAnimationNum() const { return animationNum; }

	void SetAnimIndex(int index) { animIndex = index; }

	void PlayStopAnimation(bool play) { playAnimation = play; }

	void SetAnimationIndexLimit(int limit) { anim1IndexLimit = limit; }
	int GetAnimationIndexLimit() const { return anim1IndexLimit; }

	void SetAnimationIndex(int index) { animIndex = index; }

	glm::mat4 GetModelMatrix() const { return modelMatrix; }

	std::shared_ptr<Mesh> GetMesh() const { return mesh; }
	std::shared_ptr<Material> GetMaterial() const { return material; }

private:
	//texture
	std::string texturePath;
	std::shared_ptr<Material> material;
	std::shared_ptr<Image> texture;
	std::shared_ptr<Mesh> mesh;
	glm::vec4 color = glm::vec4(1.0f);
	Shaders * shader;
	glm::mat4 projection;
	glm::mat4 modelMatrix;

	bool useAnimation = false;
	float indexTimer = 0.0f;
	float animSpeed = 0.0f;
	int animIndex = 0;
	int anim1IndexLimit = 0;
	bool playAnimation = true;

	int animationNum = 0;

protected:

	friend class SceneSerializer;

    YAML::Node encode() override {  
       YAML::Node node = Component::encode();  

       node["use_animation"] = useAnimation;  
       node["sprite_size"] = std::vector<float>{ material->spriteSize.x, material->spriteSize.y};
       node["sheet_size"] = std::vector<float>{ material->sheetSize.x, material->sheetSize.y};
       node["sprite_offset"] = std::vector<float>{ material->spriteOffset.x, material->spriteOffset.y};
       node["anim_speed"] = animSpeed; 
	   node["anim_index"] = animIndex;
	   node["anim_index_limit"] = anim1IndexLimit;
       node["color"] = std::vector<float>{color.r, color.g, color.b, color.a};

	   if (texture) {
		   node["texture_path"] = texturePath;
	   }

       return node;  
    }  

    bool decode(const YAML::Node& node) override {  
		if (!Component::decode(node)) {
			return false;
		}

		if (node["texture_path"]) {
			texturePath = node["texture_path"].as<std::string>();
			SetTexture(texturePath);
		}

		useAnimation = node["use_animation"].as<bool>();

		if (node["sprite_size"]) {
			auto spriteSizeVec = node["sprite_size"].as<std::vector<float>>();
			material->spriteSize = glm::vec2(spriteSizeVec[0], spriteSizeVec[1]);
		}

		if (node["sheet_size"]) {
			auto sheetSizeVec = node["sheet_size"].as<std::vector<float>>();
			material->sheetSize = glm::vec2(sheetSizeVec[0], sheetSizeVec[1]);
		}

	   if (node["anim_index"]) {
		   animIndex = node["anim_index"].as<int>();
	   }

	   if (node["anim_index_limit"]) {
		   anim1IndexLimit = node["anim_index_limit"].as<int>();
	   }

	   if (node["color"]) {
		   auto colorVec = node["color"].as<std::vector<float>>();
		   color = glm::vec4(colorVec[0], colorVec[1], colorVec[2], colorVec[3]);
	   }

       /*shader = node["shader"].as<Shaders*>();*/  
       /*mesh = node["mesh"].as<std::shared_ptr<Mesh>>();*/  

		if (node["anim_speed"]) {
			animSpeed = node["anim_speed"].as<float>();
		}

		if (mesh) {
			LoadMesh();
		}

		return true;
    }
};

