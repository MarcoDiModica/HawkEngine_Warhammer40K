#pragma once
#include "../MyGameEngine/Component.h"
#include "Animator.h"
#include "Animation.h"

class SkeletalAnimationComponent : public Component
{
public:
    SkeletalAnimationComponent(GameObject* owner);
    virtual ~SkeletalAnimationComponent() = default;

    SkeletalAnimationComponent(const SkeletalAnimationComponent& other);
    SkeletalAnimationComponent& operator=(const SkeletalAnimationComponent& other);


    SkeletalAnimationComponent(SkeletalAnimationComponent&& other) noexcept;
    SkeletalAnimationComponent& operator=(SkeletalAnimationComponent&& other) noexcept;


    void Start() override;

    void Update(float deltaTime) override;

    void Destroy() override;

    ComponentType GetType() const override {
        return ComponentType::ANIMATION; // Cambia a un tipo específico si es necesario
    }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
        return std::make_unique<SkeletalAnimationComponent>(new_owner);
    }

    void SetAnimation(Animation* animation) {
        testAnimation = std::make_unique<Animation>(*animation);
    }

	void AddAnimation(Animation* animation) {
		animations.push_back(std::make_unique<Animation>(*animation));
	}

	std::vector<std::unique_ptr<Animation>>& GetAnimations() {
		return animations;
	}

    Animation* GetAnimation() const {
        return testAnimation.get();
    }

    void SetAnimator(Animator* animatorr) {
        animator = std::make_unique<Animator>(*animatorr);
    }

    Animator* GetAnimator() const {
        return animator.get();
    }

	void SetAnimationPlayState(bool play) 
    {
		isPlaying = play;
	}

	bool GetAnimationPlayState() const 
    {
		return isPlaying;
	}

    int GetAnimationIndex();

	void SetAnimationIndex(int index) {
		animationIndex = index;
	}

	void PlayIndexAnimation(int index) 
    {
		if (index < 0 || index >= animations.size())
		{
			return;
		}
        SetAnimationIndex(index);
		SetAnimation(animations[index].get());
		animator->PlayAnimation(testAnimation.get());
	}

    void SetAnimationSpeed(float speed) 
    {
		animator->SetPlaySpeed(speed);
    }

	float GetAnimationSpeed()
	{
		return animator->GetPlaySpeed();
	}


    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

private:
    std::unique_ptr<Animator> animator;
    std::unique_ptr<Animation> testAnimation;
    //vector of unitque ptr of animations
	std::vector<std::unique_ptr<Animation>> animations;
    int animationIndex = 0;
	bool isPlaying = true;
 

protected:
    friend class SceneSerializer;

        YAML::Node encode() override {
            YAML::Node node = Component::encode();

            // Serializar las animaciones
            for (const auto& animation : animations) {
                if (animation) {
                    YAML::Node animNode;
                    animNode["name"] = animation->GetName();
                    animNode["duration"] = animation->GetDuration2();
                    animNode["ticks_per_second"] = animation->GetTicksPerSecond2();
                    for (const auto& animName : animation->GetAnimations()) {
                        animNode["animation_names"].push_back(animName);
                    }
                    //for (const auto& [key, value] : animation->GetBoneIDMap()) {
                    //    YAML::Node boneInfoNode;
                    //    boneInfoNode["id"] = value.id;
                    //    boneInfoNode["offset"] = value.offset;
                    //    /*node["bone_info_map"][key] = boneInfoNode;*/
                    //}
                    //node["root_node"] = animation->GetRootNode();
                    node["animations"].push_back(animNode);
                }
            }

            return node;
        }
        bool decode(const YAML::Node& node) override {
            if (!Component::decode(node))
                return false;

            // Limpiar vector de animaciones antes de agregar nuevas
            animations.clear();

            if (node["animations"]) {
                for (const auto& animNode : node["animations"]) {
                    auto animation = std::make_unique<Animation>();

                    // Extraer manualmente los datos del nodo YAML
                    if (animNode["name"]) {
                        animation->GetName() = animNode["name"].as<std::string>();
                    }
                    if (animNode["duration"]) {
                        animation->GetDuration2() = animNode["duration"].as<float>();
                    }
                    if (animNode["ticks_per_second"]) {
                        animation->GetTicksPerSecond2() = animNode["ticks_per_second"].as<int>();
                    }
                    if (animNode["animation_names"]) {
                        for (const auto& animName : animNode["animation_names"]) {
                            animation->GetAnimations().push_back(animName.as<std::string>());
                        }
                    }

       //             if (node["bone_info_map"]) {
       //                 for (auto it = node["bone_info_map"].begin(); it != node["bone_info_map"].end(); ++it) {
       //                     BoneInfo boneInfo;
       //                     boneInfo.id = it->second["id"].as<int>();
       //                     boneInfo.offset = it->second["offset"].as<glm::mat4>();
							////animation->m_BoneInfoMap[it->first.as<std::string>()] = boneInfo;
       //                     
       //                 }
       //             }

                   /* if (node["root_node"]) {
                        animation->m_RootNode = node["root_node"].as<AssimpNodeData>();
                    }*/
                     
                    // Agregar la animación al vector
                    animations.push_back(std::move(animation));
                }
            }
			testAnimation = std::make_unique<Animation>(*animations[0]);
            Start();

         

            return true;
        }
};

