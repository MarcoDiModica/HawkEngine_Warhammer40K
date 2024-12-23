#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include "types.h"
#ifdef YAML_CPP_DLL_EXPORTS
#define YAML_CPP_API __declspec(dllexport)
#else
#define YAML_CPP_API __declspec(dllimport)
#endif
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

    const auto& GetMatrix() const { return matrix; }
    const auto& GetLocalMatrix() const { return local_matrix; }
    const auto& GetLeft() const { return left; }
    const auto& GetUp() const { return up; }
    const auto& GetForward() const { return forward; }
    const auto& GetPosition() const { return position; }
    auto& GetPosition() { return position; }

    //idk why declaring it on the cpp gives me errors ;p
    glm::dquat GetRotation() const {
        return glm::quat_cast(matrix);
    }
    glm::dvec3 GetScale() const {
        return glm::dvec3(
            glm::length(left),
            glm::length(up),
            glm::length(forward)
        );
    }
    glm::dvec3 GetEulerAngles() const {
        return glm::degrees(glm::eulerAngles(GetRotation()));
    }

    auto& GetLocalPosition() { return local_matrix; }

    const auto* GetData() const { return &matrix[0][0]; }

    void Translate(const glm::dvec3& translation);
    void SetPosition(const glm::dvec3& position);
    void Rotate(double rads, const glm::dvec3& axis);
    void Scale(const glm::dvec3& scale);
    void LookAt(const glm::dvec3& target);
    void AlignToGlobalUp(const glm::vec3& worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
    void SetRotation(const glm::dvec3& eulerAngles);
    void SetRotationQuat(const glm::dquat& rotation);
    void SetScale(const glm::dvec3& scale);
    void SetForward(const glm::dvec3& forward);
    void SetMatrix(const glm::dmat4& newMatrix) {
        matrix = newMatrix;
        HandleLocalUpdate();
    }
    void SetLocalMatrix(const glm::dmat4& newMatrix) {
        local_matrix = newMatrix;
        HandleWorldUpdate();
    }

    ComponentType GetType() const override { return ComponentType::TRANSFORM; }

    Transform_Component operator*(const glm::dmat4& other) const {
        Transform_Component result(*this);
        result.matrix = matrix * other;
        return result;
    }

    Transform_Component operator*(const Transform_Component& other) const {
        Transform_Component result(*this);
        result.matrix = matrix * other.matrix;
        return result;
    }

    void TranslateLocal(const glm::dvec3& translation);

    void RotateLocal(double rads, const glm::dvec3& axis);

    void SetLocalPosition(const glm::dvec3& position) {

        local_matrix[3] = glm::dvec4(position, 1.0);
    }

protected:
    friend class SceneSerializer;

    YAML::Node encode();

    YAML::Node encodePosition() {
        YAML::Node node;
        auto position = GetPosition();

        return YAML::convert<glm::dvec3>::encode(position);
    }

    YAML::Node encodeRotation() {
        YAML::Node node;
        auto rotation = GetEulerAngles();

        return YAML::convert<glm::dvec3>::encode(rotation);

    }

    YAML::Node encodeScale() {
        YAML::Node node;
        auto scale = GetScale();

        return YAML::convert<glm::dvec3>::encode(scale);
    }

    bool decode(const YAML::Node& node);

private:

    friend class Root;
    friend class GameObject;

    union
    {
        //DONT modify directly, use SetMatrix
        glm::dmat4 matrix = glm::dmat4(1.0);
        struct
        {
            glm::dvec3 left; glm::dmat4::value_type left_w;
            glm::dvec3 up; glm::dmat4::value_type up_w;
            glm::dvec3 forward; glm::dmat4::value_type fwd_w;
            glm::dvec3 position; glm::dmat4::value_type pos_w;
        };
    };

    union
    {
        //DONT modify directly, use SetMatrix
        glm::dmat4 local_matrix = glm::dmat4(1.0);
        struct
        {
            glm::dvec3 left; glm::dmat4::value_type left_w;
            glm::dvec3 up; glm::dmat4::value_type up_w;
            glm::dvec3 forward; glm::dmat4::value_type fwd_w;
            glm::dvec3 position; glm::dmat4::value_type pos_w;
        };
    };

    // DONT modigy directly use SetLocalMatrix
    // glm::dmat4 local_matrix = glm::dmat4(1.0);

    /* Update the world matrix based on the parent's world matrix */
    void UpdateWorldMatrix(const glm::dmat4& parentWorldMatrix) {
        auto buff = matrix;
        matrix = parentWorldMatrix * local_matrix;
        if (buff != matrix) {
            int u = 7;
        }
    }
	friend class GameObject;
    void UpdateLocalMatrix(const glm::dmat4& parentWorldMatrix) {
        local_matrix = glm::inverse( parentWorldMatrix  ) * matrix;
    }

    void HandleWorldUpdate();

    void HandleLocalUpdate();

};

inline Transform_Component operator*(const glm::dmat4& m, const Transform_Component& t) {
    return Transform_Component(t) * m;
}