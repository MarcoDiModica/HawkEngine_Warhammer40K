#define GLM_ENABLE_EXPERIMENTAL
#include "TransformComponent.h"
#include "GameObject.h"
#include "../MyGameEditor/Log.h"
#include "types.h"
#include "Component.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyScriptingEngine/EngineBinds.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>

namespace Utils {
	static YAML::Node encodeMat(const glm::dmat4& rhs) {
		YAML::Node node;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				node["row" + std::to_string(i)]["col" + std::to_string(j)] = rhs[i][j];
			}
		}
		return node;
	}

	static bool decodeMat(const YAML::Node& node, glm::dmat4& rhs) {
		YAML::Node matNode = node["matrix"];
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::string rowKey = "row" + std::to_string(i);
				std::string colKey = "col" + std::to_string(j);
				if (!matNode[rowKey] || !matNode[rowKey][colKey]) {
					return false;
				}
				rhs[i][j] = matNode[rowKey][colKey].as<double>();
			}
		}
		return true;
	}
}

Transform_Component::Transform_Component(GameObject* owner) : Component(owner) { name = "Transform_Component"; }
{
    localPosition = glm::dvec3(0.0);
    localRotation = glm::dquat(1.0, 0.0, 0.0, 0.0);
    localScale = glm::dvec3(1.0);
    RecalculateLocalMatrix();
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->GetMatrix();
        worldMatrix = parentWorld * localMatrix;
    }
    else {
        worldMatrix = localMatrix;
    }
}

Transform_Component::Transform_Component(const Transform_Component& other) : Component(other)
{
    localPosition = other.localPosition;
    localRotation = other.localRotation;
    localScale = other.localScale;
    localMatrix = other.localMatrix;
    worldMatrix = other.worldMatrix;
}

Transform_Component& Transform_Component::operator=(const Transform_Component& other)
{
    if (this != &other) {
        Component::operator=(other);
        localPosition = other.localPosition;
        localRotation = other.localRotation;
        localScale = other.localScale;
        localMatrix = other.localMatrix;
        worldMatrix = other.worldMatrix;
    }
    return *this;
}

Transform_Component::Transform_Component(Transform_Component&& other) noexcept : Component(std::move(other))
{
    localPosition = std::move(other.localPosition);
    localRotation = std::move(other.localRotation);
    localScale = std::move(other.localScale);
    localMatrix = std::move(other.localMatrix);
    worldMatrix = std::move(other.worldMatrix);
}

Transform_Component& Transform_Component::operator=(Transform_Component&& other) noexcept
{
    if (this != &other)
    {
        Component::operator=(std::move(other));
        localPosition = std::move(other.localPosition);
        localRotation = std::move(other.localRotation);
        localScale = std::move(other.localScale);
        localMatrix = std::move(other.localMatrix);
        worldMatrix = std::move(other.worldMatrix);
    }
    return *this;
}

std::unique_ptr<Component> Transform_Component::Clone(GameObject* owner)
{
    auto clone = std::make_unique<Transform_Component>(*this);
    clone->owner = owner;
    return clone;
}

void Transform_Component::RecalculateLocalMatrix()
{
    glm::dmat4 T = glm::translate(glm::dmat4(1.0), localPosition);
    glm::dmat4 R = glm::toMat4(localRotation);
    glm::dmat4 S = glm::scale(glm::dmat4(1.0), localScale);
    localMatrix = T * R * S;
}

void Transform_Component::UpdateWorldMatrix()
{
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->GetMatrix();
        worldMatrix = parentWorld * localMatrix;
    }
    else {
        worldMatrix = localMatrix;
    }
}

void Transform_Component::Update(float deltaTime)
{
    UpdateWorldMatrix();
    if (owner) {
        for (auto& child : owner->GetChildren()) {
            child->GetTransform()->Update(0.0f);
        }
    }
}

// Sets the absolute world position by computing the corresponding local position.
void Transform_Component::SetPosition(const glm::dvec3& newPos)
{
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->GetMatrix();
        glm::dmat4 invParent = glm::inverse(parentWorld);
        glm::dvec4 localPos = invParent * glm::dvec4(newPos, 1.0);
        localPosition = glm::dvec3(localPos);
    }
    else {
        localPosition = newPos;
    }
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::Translate(const glm::dvec3& translation)
{
    // Translation in world space: adjust localPosition accordingly.
    if (owner && owner->GetParent()) {
        glm::dmat4 parentWorld = owner->GetParent()->GetTransform()->GetMatrix();
        glm::dmat4 invParent = glm::inverse(parentWorld);
        glm::dvec4 localTrans = invParent * glm::dvec4(translation, 0.0);
        localPosition += glm::dvec3(localTrans);
    }
    else {
        localPosition += translation;
    }
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::TranslateLocal(const glm::dvec3& translation)
{
    localPosition += translation;
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::SetRotation(const glm::dvec3& eulerAngles)
{
    // Euler angles are assumed to be in radians.
    localRotation = glm::quat(eulerAngles);
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::SetRotationQuat(const glm::dquat& rotation)
{
    localRotation = rotation;
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::Rotate(double rads, const glm::dvec3& axis)
{
    glm::dquat delta = glm::angleAxis(rads, glm::normalize(axis));
    if (owner && owner->GetParent()) {
        glm::dquat parentRot = owner->GetParent()->GetTransform()->GetRotation();
        glm::dquat localDelta = glm::inverse(parentRot) * delta * parentRot;
        localRotation = localDelta * localRotation;
    }
    else {
        localRotation = delta * localRotation;
    }
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::RotateLocal(double rads, const glm::dvec3& axis)
{
    glm::dquat delta = glm::angleAxis(rads, glm::normalize(axis));
    localRotation = delta * localRotation;
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::SetScale(const glm::dvec3& scale)
{
    localScale = scale;
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::SetForward(const glm::dvec3& newFwd)
{
    glm::dvec3 normFwd = glm::normalize(newFwd);
    glm::dvec3 refUp = glm::dvec3(0, 1, 0);
    glm::dvec3 newRight = glm::normalize(glm::cross(refUp, normFwd));
    glm::dvec3 newUp = glm::cross(normFwd, newRight);
    glm::dmat3 rotMat(newRight, newUp, normFwd);
    localRotation = glm::quat_cast(glm::dmat4(rotMat));
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::LookAt(const glm::dvec3& target)
{
    glm::dvec3 currentPos = GetPosition();
    glm::dmat4 view = glm::lookAt(currentPos, target, glm::dvec3(0, 1, 0));
    glm::dmat4 world = glm::inverse(view);
    glm::dvec3 skew;
    glm::dvec4 perspective;
    glm::decompose(world, localScale, localRotation, localPosition, skew, perspective);
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::AlignToGlobalUp(const glm::vec3& worldUp)
{
    glm::dvec3 fwd = glm::normalize(localRotation * glm::dvec3(0, 0, 1));
    glm::dvec3 right = glm::normalize(glm::cross(glm::dvec3(worldUp), fwd));
    glm::dvec3 newUp = glm::cross(fwd, right);
    glm::dmat3 rotMat(right, newUp, fwd);
    localRotation = glm::quat_cast(glm::dmat4(rotMat));
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform_Component::SetMatrix(const glm::dmat4& newMatrix)
{
    glm::dvec3 skew;
    glm::dvec4 perspective;
    glm::decompose(newMatrix, localScale, localRotation, localPosition, skew, perspective);
    RecalculateLocalMatrix();
    UpdateWorldMatrix();
}

YAML::Node Transform_Component::encode()
{
    YAML::Node node = Component::encode();
    node["localPosition"] = YAML::convert<glm::dvec3>::encode(localPosition);
    node["localRotation"] = YAML::convert<glm::dvec3>::encode(glm::degrees(glm::eulerAngles(localRotation)));
    node["localScale"] = YAML::convert<glm::dvec3>::encode(localScale);
    return node;
}

bool Transform_Component::decode(const YAML::Node& node)
{
    Component::decode(node);
    if (node["localPosition"] && node["localRotation"] && node["localScale"]) {
        YAML::convert<glm::dvec3>::decode(node["localPosition"], localPosition);
        glm::dvec3 eulerRot;
        YAML::convert<glm::dvec3>::decode(node["localRotation"], eulerRot);
        localRotation = glm::quat(glm::radians(eulerRot));
        YAML::convert<glm::dvec3>::decode(node["localScale"], localScale);
        RecalculateLocalMatrix();
        UpdateWorldMatrix();
        return true;
    }
    return false;
}

MonoObject* Transform_Component::GetSharp()
{
    if (CsharpReference) {
        return CsharpReference;
    }
    MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Transform");
    if (!klass) {
        return nullptr;
    }
    MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
    if (!monoObject) {
        return nullptr;
    }
    MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.Transform:.ctor(uintptr,HawkEngine.GameObject)", true);
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
