#include "ComponentMapper.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyPhysicsEngine/BoxColliderComponent.h"
#include "../MyGameEditor/Log.h" //QUITAR
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UIButtonComponent.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "../MyParticlesEngine/ParticleFX.h"
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include <MyPhysicsEngine/MeshColliderComponent.h>
#include <MyPhysicsEngine/CapsuleColliderComponent.h>

ComponentMapper::ComponentMapper() {
    AddMapping(std::type_index(typeid(Transform_Component)), "Transform");
	AddMapping(std::type_index(typeid(MeshRenderer)), "MeshRenderer");
    AddMapping(std::type_index(typeid(CameraComponent)), "Camera");
    AddMapping(std::type_index(typeid(MeshColliderComponent)), "MeshCollider");
    AddMapping(std::type_index(typeid(CapsuleColliderComponent)), "CapsuleCollider");
    AddMapping(std::type_index(typeid(BoxColliderComponent)), "BoxCollider");
    AddMapping(std::type_index(typeid(BaseColliderComponent)), "Collider");
    AddMapping(std::type_index(typeid(RigidbodyComponent)), "Rigidbody");
    AddMapping(std::type_index(typeid(SoundComponent)), "Audio");
    AddMapping(std::type_index(typeid(UIImageComponent)), "UIImage");
	AddMapping(std::type_index(typeid(UIButtonComponent)), "UIButton");
	AddMapping(std::type_index(typeid(UICanvasComponent)), "UICanvas");
	AddMapping(std::type_index(typeid(UITransformComponent)), "UITransform");
	AddMapping(std::type_index(typeid(SkeletalAnimationComponent)), "SkeletalAnimation");
	AddMapping(std::type_index(typeid(ParticleFX)), "ParticleFX");
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