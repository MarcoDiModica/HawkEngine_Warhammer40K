#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include "types.h"
#include <yaml-cpp/yaml.h>

class SceneSerializer;

class Transform_Component : public Component
{
public:
    Transform_Component(GameObject* owner);
    ~Transform_Component() override = default;

    Transform_Component(const Transform_Component& other);
    Transform_Component& operator=(const Transform_Component& other);

    Transform_Component(Transform_Component&& other) noexcept;
    Transform_Component& operator=(Transform_Component&& other) noexcept;

    void Start() override {}
    void Update(float deltaTime) override;
    void Destroy() override {}

    std::unique_ptr<Component> Clone(GameObject* owner) override;

    // World (final) transform matrix (read-only)
    const glm::dmat4& GetMatrix() const { return worldMatrix; }
    // Local transform matrix (computed from canonical data)
    const glm::dmat4& GetLocalMatrix() const { return localMatrix; }

    // Accessors for canonical transform components
    glm::dvec3 GetPosition() const { return glm::dvec3(worldMatrix[3]); }
    glm::dquat GetRotation() const { return localRotation; }
    glm::dvec3 GetScale() const { return localScale; }
    glm::dvec3 GetEulerAngles() const {
        return glm::degrees(glm::eulerAngles(localRotation));
    }

    // --- New helper functions ---
    // Returns the forward vector (local space, i.e. rotation applied to (0,0,1))
    glm::dvec3 GetForward() const {
        return glm::normalize(localRotation * glm::dvec3(0, 0, 1));
    }
    // Returns the up vector (local space, i.e. rotation applied to (0,1,0))
    glm::dvec3 GetUp() const {
        return glm::normalize(localRotation * glm::dvec3(0, 1, 0));
    }
    // Returns a pointer to the world matrix data (for shader upload, etc.)
    const double* GetData() const {
        return &worldMatrix[0][0];
    }

    // Inspector API functions that update the canonical local transform
    void SetPosition(const glm::dvec3& newPos);
    void Translate(const glm::dvec3& translation);
    void TranslateLocal(const glm::dvec3& translation);

    void SetRotation(const glm::dvec3& eulerAngles); // expects radians
    void SetRotationQuat(const glm::dquat& rotation);
    void Rotate(double rads, const glm::dvec3& axis);
    void RotateLocal(double rads, const glm::dvec3& axis);

    void SetScale(const glm::dvec3& scale);

    void SetForward(const glm::dvec3& newFwd);
    void LookAt(const glm::dvec3& target);
    void AlignToGlobalUp(const glm::vec3& worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

    // For serialization: directly set the transform by decomposing the matrix.
    void SetMatrix(const glm::dmat4& newMatrix);

    // --- Compatibility wrappers ---
    // Instead of directly modifying the local matrix, use the canonical local components.
    glm::dvec3 GetLocalPosition() const { return localPosition; }
	void SetLocalPosition(const glm::dvec3& pos) {
		localPosition = pos;
		RecalculateLocalMatrix();
		UpdateWorldMatrix();
	}

	glm::dvec3 GetWorldPosition() const {
		return glm::dvec3(worldMatrix[3]);
	}
    // If your code was calling UpdateLocalMatrix(), use this wrapper:
    void UpdateLocalMatrix() { RecalculateLocalMatrix(); UpdateWorldMatrix(); }
    // For legacy code that set the local matrix directly, delegate to SetMatrix()
    void SetLocalMatrix(const glm::dmat4& mat) { SetMatrix(mat); }

    MonoObject* CsharpReference = nullptr;
    MonoObject* GetSharp() override;

    ComponentType GetType() const override { return ComponentType::TRANSFORM; }

    void PreserveWorldTransform();

protected:
    friend class SceneSerializer;
    YAML::Node encode();
    bool decode(const YAML::Node& node);

private:
    // Canonical local transform components
    glm::dvec3 localPosition = glm::dvec3(0.0);
    glm::dquat localRotation = glm::dquat(1.0, 0.0, 0.0, 0.0);
    glm::dvec3 localScale = glm::dvec3(1.0);

    // Computed matrices from the canonical data
    glm::dmat4 localMatrix = glm::dmat4(1.0);
    glm::dmat4 worldMatrix = glm::dmat4(1.0);

    // Helper functions to rebuild the matrices from the canonical data.
    void RecalculateLocalMatrix();
    void UpdateWorldMatrix();
};
