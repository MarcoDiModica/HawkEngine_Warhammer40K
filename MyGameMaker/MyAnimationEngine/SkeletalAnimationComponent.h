#pragma once
#include "../MyGameEngine/Component.h"
#include "Animator.h"
#include "Animation.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    void TransitionAnimations(int oldAnim, int newAnim, float timeToTransition);

    ComponentType GetType() const override {
        return ComponentType::ANIMATION; // Cambia a un tipo espec�fico si es necesario
    }

    std::unique_ptr<Component> Clone(GameObject* new_owner) override {
        return std::make_unique<SkeletalAnimationComponent>(new_owner);
    }

    void SetAnimation(Animation* animation) {
        animation1 = std::make_unique<Animation>(*animation);
    }

	void AddAnimation(Animation* animation) {
		animations.push_back(std::make_unique<Animation>(*animation));
	}

	std::vector<std::unique_ptr<Animation>>& GetAnimations() {
		return animations;
	}

    Animation* GetAnimation() const {
        return animation1.get();
    }

    void SetAnimator(Animator* animatorr) {
        animator = std::make_unique<Animator>(*animatorr);
    }

    Animator* GetAnimator() const {
        return animator.get();
    }

	float GetAnimationDuration() const {
		return animation1->GetDuration();
	}

	float GetAnimationTime() const {
		return animator->GetCurrentMTime();
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
		animator->PlayAnimation(animation1.get());
	}

    void SetAnimationSpeed(float speed) 
    {
		animator->SetPlaySpeed(speed);
    }

    void SetNewAnimation(Animation* animation) {
        newAnimation = std::make_unique<Animation>(*animation);
    }

	float GetAnimationSpeed()
	{
		return animator->GetPlaySpeed();
	}


    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;


    float blendFactor = 0.0f;
    bool isBlending = false;
	void SaveBinary(const std::string& filename) const;
	bool LoadBinary(const std::string& filename);

	void WriteAssimpNodeData(std::ofstream& fout, const AssimpNodeData& node) const;
	static AssimpNodeData ReadAssimpNodeData(std::ifstream& fin);


    std::string serializeMat4(const glm::mat4& mat) {
        std::ostringstream oss;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                oss << mat[i][j];
                if (i != 3 || j != 3) {
                    oss << " ";
                }
            }
        }
        return oss.str();
       
    }
    std::string serializeAssimpNodeData(const AssimpNodeData& node) {
        std::ostringstream oss;
        oss << serializeMat4(node.transformation) << " ";
        oss << node.name << " ";
        oss << node.childrenCount << " ";
        for (const auto& child : node.children) {
            oss << serializeAssimpNodeData(child) << " ";
        }
        return oss.str();
    }

    AssimpNodeData deserializeAssimpNodeData(std::istringstream& iss) {
        AssimpNodeData node;
        glm::mat4 transformation;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                iss >> transformation[i][j];
            }
        }
        node.transformation = transformation;
        iss >> node.name;
        iss >> node.childrenCount;
        for (int i = 0; i < node.childrenCount; ++i) {
            node.children.push_back(deserializeAssimpNodeData(iss));
        }
        return node;
    }
    glm::mat4 deserializeMat4(const std::string& data) {
        glm::mat4 mat;
        std::istringstream iss(data);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                iss >> mat[i][j];
            }
        }
        return mat;
    }

private:
    std::unique_ptr<Animator> animator;
    std::unique_ptr<Animation> animation1;
    std::unique_ptr<Animation> newAnimation;
    //vector of unitque ptr of animations
	std::vector<std::unique_ptr<Animation>> animations;
    int animationIndex = 0;
	bool isPlaying = true;
	float timeToTransition = 0.0f;



protected:
    friend class SceneSerializer;

    YAML::Node encode() override {
        YAML::Node node = Component::encode();

        /*for (const auto& animation : animations) {
            if (animation) {
                YAML::Node animNode;
                animNode["name"] = animation->GetName();
                animNode["duration"] = animation->GetDuration2();
                animNode["ticks_per_second"] = animation->GetTicksPerSecond2();

                for (const auto& animName : animation->GetAnimations()) {
                    animNode["animation_names"].push_back(animName);
                }

                YAML::Node boneMapNode;
                for (const auto& [key, value] : animation->GetBoneIDMap()) {
                    YAML::Node boneInfoNode;
                    boneInfoNode["id"] = value.id;

                    std::ostringstream oss;
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            oss << value.offset[i][j] << " ";
                        }
                    }
                    boneInfoNode["offset"] = oss.str(); 

                    boneMapNode[key] = boneInfoNode;
                }
                animNode["bone_info_map"] = boneMapNode;
                YAML::Node bonesNode;
                for ( auto& bone : animation->m_Bones) {
                    YAML::Node boneNode;
                    boneNode["name"] = bone.GetBoneName();
                    boneNode["parent_name"] = bone.GetParentName();
                    boneNode["id"] = bone.GetBoneID();

                    YAML::Node positionsNode;
                    for (const auto& pos : bone.m_Positions) {
                        YAML::Node posNode;
                        posNode["timestamp"] = pos.timeStamp;
                        posNode["position"] = std::vector<float>{ pos.position.x, pos.position.y, pos.position.z };
                        positionsNode.push_back(posNode);
                    }
                    boneNode["positions"] = positionsNode;

                    YAML::Node rotationsNode;
                    for (const auto& rot : bone.m_Rotations) {
                        YAML::Node rotNode;
                        rotNode["timestamp"] = rot.timeStamp;
                        rotNode["orientation"] = std::vector<float>{ rot.orientation.x, rot.orientation.y, rot.orientation.z, rot.orientation.w };
                        rotationsNode.push_back(rotNode);
                    }
                    boneNode["rotations"] = rotationsNode;

                    YAML::Node scalesNode;
                    for (const auto& scale : bone.m_Scales) {
                        YAML::Node scaleNode;
                        scaleNode["timestamp"] = scale.timeStamp;
                        scaleNode["scale"] = std::vector<float>{ scale.scale.x, scale.scale.y, scale.scale.z };
                        scalesNode.push_back(scaleNode);
                    }
                    boneNode["scales"] = scalesNode;

                    bonesNode.push_back(boneNode);
                }
                animNode["bones"] = bonesNode;
                animNode["root_node"] = serializeAssimpNodeData(animation->GetRootNode());

                node["animations"].push_back(animNode);
            }
        }*/

        std::string animName = "skeletal_anim_" +  owner->GetName();
        SaveBinary(animName);
        node["animation_file"] = animName;

        return node;
    }

    bool decode(const YAML::Node& node) override {
        if (!Component::decode(node))
            return false;

        /*animations.clear();

        if (node["animations"]) {
            for (const auto& animNode : node["animations"]) {
                auto animation = std::make_unique<Animation>();

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

                if (animNode["bone_info_map"]) {
                    for (auto it = animNode["bone_info_map"].begin(); it != animNode["bone_info_map"].end(); ++it) {
                        BoneInfo boneInfo;
                        boneInfo.id = it->second["id"].as<int>();

                        std::istringstream iss(it->second["offset"].as<std::string>());
                        glm::mat4 offsetMat;
                        for (int i = 0; i < 4; ++i) {
                            for (int j = 0; j < 4; ++j) {
                                iss >> offsetMat[i][j];
                            }
                        }
                        boneInfo.offset = offsetMat;
                        animation->m_BoneInfoMap[it->first.as<std::string>()] = boneInfo;
                    }
                }

                if (animNode["bones"] && animNode["bones"].IsSequence()) {
                    for (const auto& boneNode : animNode["bones"]) {
                        if (!boneNode["name"] || !boneNode["parent_name"] || !boneNode["id"]) {
                            continue;
                        }

                        std::string boneName = boneNode["name"].as<std::string>();
                        std::string parentName = boneNode["parent_name"].as<std::string>();
                        int boneID = boneNode["id"].as<int>();

                        std::vector<KeyPosition> positions;
                        if (boneNode["positions"]) {
                            for (const auto& posNode : boneNode["positions"]) {
                                KeyPosition pos;
                                pos.timeStamp = posNode["timestamp"].as<float>();
                                auto posVec = posNode["position"].as<std::vector<float>>();
                                pos.position = glm::vec3(posVec[0], posVec[1], posVec[2]);
                                positions.push_back(pos);
                            }
                        }

                        std::vector<KeyRotation> rotations;
                        if (boneNode["rotations"]) {
                            for (const auto& rotNode : boneNode["rotations"]) {
                                KeyRotation rot;
                                rot.timeStamp = rotNode["timestamp"].as<float>();
                                auto rotVec = rotNode["orientation"].as<std::vector<float>>();
                                rot.orientation = glm::quat(rotVec[3], rotVec[0], rotVec[1], rotVec[2]);
                                rotations.push_back(rot);
                            }
                        }

                        std::vector<KeyScale> scales;
                        if (boneNode["scales"]) {
                            for (const auto& scaleNode : boneNode["scales"]) {
                                KeyScale scale;
                                scale.timeStamp = scaleNode["timestamp"].as<float>();
                                auto scaleVec = scaleNode["scale"].as<std::vector<float>>();
                                scale.scale = glm::vec3(scaleVec[0], scaleVec[1], scaleVec[2]);
                                scales.push_back(scale);
                            }
                        }

                        aiNodeAnim* channel = new aiNodeAnim();
                        channel->mNumPositionKeys = positions.size();
                        channel->mNumRotationKeys = rotations.size();
                        channel->mNumScalingKeys = scales.size();

                        channel->mPositionKeys = new aiVectorKey[channel->mNumPositionKeys];
                        for (size_t i = 0; i < positions.size(); ++i) {
                            channel->mPositionKeys[i].mTime = positions[i].timeStamp;
                            channel->mPositionKeys[i].mValue = aiVector3D(positions[i].position.x, positions[i].position.y, positions[i].position.z);
                        }

                        channel->mRotationKeys = new aiQuatKey[channel->mNumRotationKeys];
                        for (size_t i = 0; i < rotations.size(); ++i) {
                            channel->mRotationKeys[i].mTime = rotations[i].timeStamp;
                            channel->mRotationKeys[i].mValue = aiQuaternion(rotations[i].orientation.w, rotations[i].orientation.x, rotations[i].orientation.y, rotations[i].orientation.z);
                        }

                        channel->mScalingKeys = new aiVectorKey[channel->mNumScalingKeys];
                        for (size_t i = 0; i < scales.size(); ++i) {
                            channel->mScalingKeys[i].mTime = scales[i].timeStamp;
                            channel->mScalingKeys[i].mValue = aiVector3D(scales[i].scale.x, scales[i].scale.y, scales[i].scale.z);
                        }

                        Bone bone(boneName, parentName, boneID, channel);
                        animation->m_Bones.push_back(bone);
                    }
                }
             

                if (animNode["root_node"]) {
                    std::istringstream iss(animNode["root_node"].as<std::string>());
                    animation->m_RootNode = deserializeAssimpNodeData(iss);
                }

                animations.push_back(std::move(animation));
            }
        }

        if (!animations.empty()) {
            testAnimation = std::make_unique<Animation>(*animations[0]);
        }

        Start();*/

		if (node["animation_file"]) {
			std::string animName = node["animation_file"].as<std::string>();
			LoadBinary(animName);
		}

        return true;
    }

    
};

