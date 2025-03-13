#include "ComponentMapper.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyPhysicsEngine/ColliderComponent.h"
#include "../MyGameEditor/Log.h" //QUITAR
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"

ComponentMapper::ComponentMapper() {
    AddMapping(std::type_index(typeid(Transform_Component)), "Transform");
	AddMapping(std::type_index(typeid(MeshRenderer)), "MeshRenderer");
    AddMapping(std::type_index(typeid(CameraComponent)), "Camera");
    AddMapping(std::type_index(typeid(ColliderComponent)), "Collider");
    AddMapping(std::type_index(typeid(RigidbodyComponent)), "Rigidbody");
    AddMapping(std::type_index(typeid(SoundComponent)), "Audio");
	AddMapping(std::type_index(typeid(SkeletalAnimationComponent)), "SkeletalAnimation");
    // mas components
}

ComponentMapper::~ComponentMapper() {}

std::string ComponentMapper::GetMappedName(const std::type_index& typeIndex) const {
    auto it = mappings.find(typeIndex);
    if (it != mappings.end()) {
        return it->second;
    }
    else {
        LOG(LogType::LOG_ERROR, (std::string("No Type found: ") + typeIndex.name()).c_str());
        throw std::runtime_error("No type found: " + std::string(typeIndex.name()));
    }
}

void ComponentMapper::AddMapping(const std::type_index& typeIndex, const std::string& mappedName) {
    mappings[typeIndex] = mappedName;
}