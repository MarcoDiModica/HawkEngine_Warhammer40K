#include "SkeletalAnimationComponent.h"
#include <iostream>
#include <MyScriptingEngine/MonoManager.h>
#include <mono/metadata/debug-helpers.h>
#include <fstream>

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner) : Component(owner)
{
    name = "SkeletalAnimationComponent";
	updateInStop = false;
}

SkeletalAnimationComponent::SkeletalAnimationComponent(const SkeletalAnimationComponent& other) : Component(other) {
    if (other.animator) {
        animator = std::make_unique<Animator>(*other.animator);
    }
    if (other.animation1) {
        animation1 = std::make_unique<Animation>(*other.animation1);
    }
}

SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(const SkeletalAnimationComponent& other)
{
    if (this != &other)
    {
        Component::operator=(other);
        if (other.animator) {
            animator = std::make_unique<Animator>(*other.animator);
        } else {
            animator.reset();
        }
        if (other.animation1) {
            animation1 = std::make_unique<Animation>(*other.animation1);
        } else {
            animation1.reset();
        }
    }
    return *this;
}

SkeletalAnimationComponent::SkeletalAnimationComponent(SkeletalAnimationComponent&& other) noexcept : Component(std::move(other)) {
    animator = std::move(other.animator);
    animation1 = std::move(other.animation1);
}


SkeletalAnimationComponent& SkeletalAnimationComponent::operator=(SkeletalAnimationComponent&& other) noexcept {
    if (this != &other) {
        Component::operator=(std::move(other));
        animator = std::move(other.animator);
        animation1 = std::move(other.animation1);
    }
    return *this;
}

void SkeletalAnimationComponent::Start()
{
	if (animator == nullptr) 
    {
		animator = std::make_unique<Animator>(animation1.get());
	}
    //animator = std::make_unique<Animator>(testAnimation.get());
    animator->PlayAnimation(animation1.get());

}

void SkeletalAnimationComponent::Update(float deltaTime)
{
    if (isPlaying) 
    {
        if (isBlending) 
        {
            animator->TransitionToAnimation(animation1.get(), newAnimation.get(), timeToTransition, deltaTime);
			//animator->BlendTwoAnimations(animations[0].get(), animations[2].get(), blendFactor, deltaTime);
        }
        else 
        {
            animator->UpdateAnimation(deltaTime);
        }
        
    }
}

void SkeletalAnimationComponent::TransitionAnimations(int oldAnim, int newAnim, float timeToTransitionAnim) 
{
    animation1 = std::make_unique<Animation>(*animations[oldAnim].get());
    newAnimation = std::make_unique<Animation>(*animations[newAnim].get());
	timeToTransition = timeToTransitionAnim;
    animator->SetTransitionTime(0);
	isBlending = true;
}

void SkeletalAnimationComponent::Destroy()
{
}

int SkeletalAnimationComponent::GetAnimationIndex() {
    return animationIndex;
}

MonoObject* SkeletalAnimationComponent::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "SkeletalAnimation");
    if (!klass) {
        return nullptr;
    }
    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
    if (!monoObject) {
        return nullptr;
    }
    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.SkeletalAnimation:.ctor(uintptr,HawkEngine.GameObject)", true);
    MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
    if (!method)
    {
        return nullptr;
    }
    uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
    MonoObject* ownerGo = owner ? owner->GetSharp() : nullptr;
    if (!ownerGo)
    {
        return nullptr;
    }
    void* args[2];
    args[0] = &componentPtr;
    args[1] = ownerGo;
    mono_runtime_invoke(method, monoObject, args, NULL);
    CsharpReference = monoObject;
    return CsharpReference;
}

void SkeletalAnimationComponent::SaveBinary(const std::string& filename) const
{
	std::string fullPath = "Library/Animation/" + filename + ".anim";

	if (!std::filesystem::exists("Library/Animation")) {
		std::filesystem::create_directory("Library/Animation");
	}

	std::ofstream fout(fullPath, std::ios::binary);
	if (!fout.is_open()) {
		LOG(LogType::LOG_ERROR, "Error al guardar la animación esqueletal: %s", fullPath.c_str());
		return;
	}

	fout.write(reinterpret_cast<const char*>(&isPlaying), sizeof(isPlaying));
	fout.write(reinterpret_cast<const char*>(&animationIndex), sizeof(animationIndex));

	uint32_t numAnimations = static_cast<uint32_t>(animations.size());
	fout.write(reinterpret_cast<const char*>(&numAnimations), sizeof(numAnimations));

	for (const auto& animation : animations) {
		if (animation) {
			uint32_t nameLength = static_cast<uint32_t>(animation->GetName().length());
			fout.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			fout.write(animation->GetName().c_str(), nameLength);

			float duration = animation->GetDuration2();
			int ticksPerSecond = animation->GetTicksPerSecond2();
			fout.write(reinterpret_cast<const char*>(&duration), sizeof(duration));
			fout.write(reinterpret_cast<const char*>(&ticksPerSecond), sizeof(ticksPerSecond));

			const auto& animNames = animation->GetAnimations();
			uint32_t numAnimNames = static_cast<uint32_t>(animNames.size());
			fout.write(reinterpret_cast<const char*>(&numAnimNames), sizeof(numAnimNames));
			for (const auto& name : animNames) {
				uint32_t animNameLength = static_cast<uint32_t>(name.length());
				fout.write(reinterpret_cast<const char*>(&animNameLength), sizeof(animNameLength));
				fout.write(name.c_str(), animNameLength);
			}

			const auto& boneMap = animation->GetBoneIDMap();
			uint32_t numBones = static_cast<uint32_t>(boneMap.size());
			fout.write(reinterpret_cast<const char*>(&numBones), sizeof(numBones));

			for (const auto& [boneName, boneInfo] : boneMap) {
				uint32_t boneNameLength = static_cast<uint32_t>(boneName.length());
				fout.write(reinterpret_cast<const char*>(&boneNameLength), sizeof(boneNameLength));
				fout.write(boneName.c_str(), boneNameLength);

				fout.write(reinterpret_cast<const char*>(&boneInfo.id), sizeof(boneInfo.id));

				fout.write(reinterpret_cast<const char*>(&boneInfo.offset), sizeof(glm::mat4));
			}

			uint32_t numBonesList = static_cast<uint32_t>(animation->m_Bones.size());
			fout.write(reinterpret_cast<const char*>(&numBonesList), sizeof(numBonesList));

			for (auto& bone : animation->m_Bones) {
				uint32_t boneNameLength = static_cast<uint32_t>(bone.GetBoneName().length());
				fout.write(reinterpret_cast<const char*>(&boneNameLength), sizeof(boneNameLength));
				fout.write(bone.GetBoneName().c_str(), boneNameLength);

				uint32_t parentNameLength = static_cast<uint32_t>(bone.GetParentName().length());
				fout.write(reinterpret_cast<const char*>(&parentNameLength), sizeof(parentNameLength));
				fout.write(bone.GetParentName().c_str(), parentNameLength);

				int boneID = bone.GetBoneID();
				fout.write(reinterpret_cast<const char*>(&boneID), sizeof(boneID));

				const auto& positions = bone.m_Positions;
				uint32_t numPositions = static_cast<uint32_t>(positions.size());
				fout.write(reinterpret_cast<const char*>(&numPositions), sizeof(numPositions));
				for (const auto& pos : positions) {
					fout.write(reinterpret_cast<const char*>(&pos.timeStamp), sizeof(float));
					fout.write(reinterpret_cast<const char*>(&pos.position), sizeof(glm::vec3));
				}

				const auto& rotations = bone.m_Rotations;
				uint32_t numRotations = static_cast<uint32_t>(rotations.size());
				fout.write(reinterpret_cast<const char*>(&numRotations), sizeof(numRotations));
				for (const auto& rot : rotations) {
					fout.write(reinterpret_cast<const char*>(&rot.timeStamp), sizeof(float));
					fout.write(reinterpret_cast<const char*>(&rot.orientation), sizeof(glm::quat));
				}

				const auto& scales = bone.m_Scales;
				uint32_t numScales = static_cast<uint32_t>(scales.size());
				fout.write(reinterpret_cast<const char*>(&numScales), sizeof(numScales));
				for (const auto& scale : scales) {
					fout.write(reinterpret_cast<const char*>(&scale.timeStamp), sizeof(float));
					fout.write(reinterpret_cast<const char*>(&scale.scale), sizeof(glm::vec3));
				}
			}

			WriteAssimpNodeData(fout, animation->GetRootNode());
		}
	}

	LOG(LogType::LOG_INFO, "Animación esqueletal guardada correctamente: %s", fullPath.c_str());
}

void SkeletalAnimationComponent::WriteAssimpNodeData(std::ofstream& fout, const AssimpNodeData& node) const
{
	fout.write(reinterpret_cast<const char*>(&node.transformation), sizeof(glm::mat4));

	uint32_t nameLength = static_cast<uint32_t>(node.name.length());
	fout.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
	fout.write(node.name.c_str(), nameLength);

	fout.write(reinterpret_cast<const char*>(&node.childrenCount), sizeof(node.childrenCount));

	for (const auto& child : node.children) {
		WriteAssimpNodeData(fout, child);
	}
}

bool SkeletalAnimationComponent::LoadBinary(const std::string& filename)
{
	std::string fullPath = "Library/Animation/" + filename + ".anim";

	std::ifstream fin(fullPath, std::ios::binary);
	if (!fin.is_open()) {
		LOG(LogType::LOG_ERROR, "Error al cargar la animación esqueletal: %s", fullPath.c_str());
		return false;
	}

	fin.read(reinterpret_cast<char*>(&isPlaying), sizeof(isPlaying));
	fin.read(reinterpret_cast<char*>(&animationIndex), sizeof(animationIndex));

	uint32_t numAnimations;
	fin.read(reinterpret_cast<char*>(&numAnimations), sizeof(numAnimations));

	animations.clear();

	for (uint32_t i = 0; i < numAnimations; ++i) {
		auto animation = std::make_unique<Animation>();

		uint32_t nameLength;
		fin.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		std::string animName(nameLength, '\0');
		fin.read(&animName[0], nameLength);
		animation->GetName() = animName;

		float duration;
		int ticksPerSecond;
		fin.read(reinterpret_cast<char*>(&duration), sizeof(duration));
		fin.read(reinterpret_cast<char*>(&ticksPerSecond), sizeof(ticksPerSecond));
		animation->GetDuration2() = duration;
		animation->GetTicksPerSecond2() = ticksPerSecond;

		uint32_t numAnimNames;
		fin.read(reinterpret_cast<char*>(&numAnimNames), sizeof(numAnimNames));
		for (uint32_t j = 0; j < numAnimNames; ++j) {
			uint32_t animNameLength;
			fin.read(reinterpret_cast<char*>(&animNameLength), sizeof(animNameLength));
			std::string name(animNameLength, '\0');
			fin.read(&name[0], animNameLength);
			animation->GetAnimations().push_back(name);
		}

		uint32_t numBones;
		fin.read(reinterpret_cast<char*>(&numBones), sizeof(numBones));

		for (uint32_t j = 0; j < numBones; ++j) {
			uint32_t boneNameLength;
			fin.read(reinterpret_cast<char*>(&boneNameLength), sizeof(boneNameLength));
			std::string boneName(boneNameLength, '\0');
			fin.read(&boneName[0], boneNameLength);

			BoneInfo boneInfo;
			fin.read(reinterpret_cast<char*>(&boneInfo.id), sizeof(boneInfo.id));
			fin.read(reinterpret_cast<char*>(&boneInfo.offset), sizeof(glm::mat4));

			animation->m_BoneInfoMap[boneName] = boneInfo;
		}

		uint32_t numBonesList;
		fin.read(reinterpret_cast<char*>(&numBonesList), sizeof(numBonesList));

		for (uint32_t j = 0; j < numBonesList; ++j) {
			uint32_t boneNameLength;
			fin.read(reinterpret_cast<char*>(&boneNameLength), sizeof(boneNameLength));
			std::string boneName(boneNameLength, '\0');
			fin.read(&boneName[0], boneNameLength);

			uint32_t parentNameLength;
			fin.read(reinterpret_cast<char*>(&parentNameLength), sizeof(parentNameLength));
			std::string parentName(parentNameLength, '\0');
			fin.read(&parentName[0], parentNameLength);

			int boneID;
			fin.read(reinterpret_cast<char*>(&boneID), sizeof(boneID));

			aiNodeAnim* channel = new aiNodeAnim();

			uint32_t numPositions;
			fin.read(reinterpret_cast<char*>(&numPositions), sizeof(numPositions));
			channel->mNumPositionKeys = numPositions;
			channel->mPositionKeys = new aiVectorKey[numPositions];

			for (uint32_t k = 0; k < numPositions; ++k) {
				KeyPosition pos;
				fin.read(reinterpret_cast<char*>(&pos.timeStamp), sizeof(float));
				fin.read(reinterpret_cast<char*>(&pos.position), sizeof(glm::vec3));

				channel->mPositionKeys[k].mTime = pos.timeStamp;
				channel->mPositionKeys[k].mValue = aiVector3D(pos.position.x, pos.position.y, pos.position.z);
			}

			uint32_t numRotations;
			fin.read(reinterpret_cast<char*>(&numRotations), sizeof(numRotations));
			channel->mNumRotationKeys = numRotations;
			channel->mRotationKeys = new aiQuatKey[numRotations];

			for (uint32_t k = 0; k < numRotations; ++k) {
				KeyRotation rot;
				fin.read(reinterpret_cast<char*>(&rot.timeStamp), sizeof(float));
				fin.read(reinterpret_cast<char*>(&rot.orientation), sizeof(glm::quat));

				channel->mRotationKeys[k].mTime = rot.timeStamp;
				channel->mRotationKeys[k].mValue = aiQuaternion(rot.orientation.w, rot.orientation.x, rot.orientation.y, rot.orientation.z);
			}

			uint32_t numScales;
			fin.read(reinterpret_cast<char*>(&numScales), sizeof(numScales));
			channel->mNumScalingKeys = numScales;
			channel->mScalingKeys = new aiVectorKey[numScales];

			for (uint32_t k = 0; k < numScales; ++k) {
				KeyScale scale;
				fin.read(reinterpret_cast<char*>(&scale.timeStamp), sizeof(float));
				fin.read(reinterpret_cast<char*>(&scale.scale), sizeof(glm::vec3));

				channel->mScalingKeys[k].mTime = scale.timeStamp;
				channel->mScalingKeys[k].mValue = aiVector3D(scale.scale.x, scale.scale.y, scale.scale.z);
			}

			Bone bone(boneName, parentName, boneID, channel);
			animation->m_Bones.push_back(bone);
		}

		animation->m_RootNode = ReadAssimpNodeData(fin);

		animations.push_back(std::move(animation));
	}

	if (!animations.empty()) {
		animation1 = std::make_unique<Animation>(*animations[animationIndex]);
	}

	Start();

	LOG(LogType::LOG_INFO, "Animación esqueletal cargada correctamente: %s", fullPath.c_str());
	return true;
}

AssimpNodeData SkeletalAnimationComponent::ReadAssimpNodeData(std::ifstream& fin)
{
	AssimpNodeData node;

	fin.read(reinterpret_cast<char*>(&node.transformation), sizeof(glm::mat4));

	uint32_t nameLength;
	fin.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
	node.name.resize(nameLength);
	fin.read(&node.name[0], nameLength);

	fin.read(reinterpret_cast<char*>(&node.childrenCount), sizeof(node.childrenCount));

	for (int i = 0; i < node.childrenCount; ++i) {
		node.children.push_back(ReadAssimpNodeData(fin));
	}

	return node;
}