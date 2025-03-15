#pragma once
#include "../MyGameEngine/Component.h"
#include "Bone.h"

class GameObject;

class BoneComponent : public Component
{
private:
	Bone* bone;

public:
	BoneComponent(GameObject* owner);
	virtual ~BoneComponent() = default;

	BoneComponent(const BoneComponent& other);
	BoneComponent& operator=(const BoneComponent& other);

	BoneComponent(BoneComponent&& other) noexcept;
	BoneComponent& operator=(BoneComponent&& other) noexcept;

	void Start() override;

	void Update(float deltaTime) override;

	void Destroy() override;

	void SetBone(Bone* bone);


	ComponentType GetType() const override {
		return ComponentType::BONE; // Cambia a un tipo específico si es necesario
	}
	std::unique_ptr<Component> Clone(GameObject* new_owner) override {
		return std::make_unique<BoneComponent>(new_owner);
	}
	MonoObject* GetSharp() override {
		return CsharpReference;
	}
	

};

