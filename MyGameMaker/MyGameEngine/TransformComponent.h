#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <yaml-cpp/yaml.h>
#include "Component.h"


class SceneSerializer;

class Transform_Component : public Component
{
public:
    Transform_Component(std::weak_ptr<GameObject> owner);
    ~Transform_Component() override = default;

    void Start() override {}
    void Update(float deltaTime) override {}
    void Destroy() override {}

    const auto& GetMatrix() const { return matrix; }
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

    const auto* GetData() const { return &matrix[0][0]; }

    void Translate(const glm::dvec3& translation);
    void Rotate(double rads, const glm::dvec3& axis);
    void Scale(const glm::dvec3& scale);
    void LookAt(const glm::dvec3& target);

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

protected:

    friend class SceneSerializer;

    YAML::Node encode() {
        YAML::Node node = Component::encode();

        node["position"] = encodePosition();
        node["rotation"] = encodeRotation();
        node["scale"] = encodeScale();
        return node;
    }


    YAML::Node encodePosition() {
        YAML::Node node;
        auto position = GetPosition();

        node["x"] = position.x;
        node["y"] = position.y;
        node["z"] = position.z;
        return node;
    }

    YAML::Node encodeRotation() {
        YAML::Node node;
        auto rotation = GetRotation();

        node["x"] = rotation.x;
        node["y"] = rotation.y;
        node["z"] = rotation.z;
        return node;
    }

    YAML::Node encodeScale() {
        YAML::Node node;
        auto scale = GetScale();

        node["x"] = scale.x;
        node["y"] = scale.y;
        node["z"] = scale.z;
        return node;
    }


private:

    union
    {
        glm::dmat4 matrix = glm::dmat4(1.0);
        struct
        {
            glm::dvec3 left; glm::dmat4::value_type left_w;
            glm::dvec3 up; glm::dmat4::value_type up_w;
            glm::dvec3 forward; glm::dmat4::value_type fwd_w;
            glm::dvec3 position; glm::dmat4::value_type pos_w;
        };
    };
};

inline Transform_Component operator*(const glm::dmat4& m, const Transform_Component& t) {
    return Transform_Component(t) * m;
}