#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <yaml-cpp/yaml.h>
#include "Component.h"
#include "types.h"

class SceneSerializer;

class Transform_Component : public Component
{
public:
    Transform_Component(std::weak_ptr<GameObject> owner);
    ~Transform_Component() override = default;

    Transform_Component(const Transform_Component& other);
    Transform_Component& operator=(const Transform_Component& other);

    void Start() override {}
    void Update(float deltaTime) override {}
    void Destroy() override {}

    std::shared_ptr<Component> Clone() override;

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
	void SetPosition(const glm::dvec3& position);
    void Rotate(double rads, const glm::dvec3& axis);
    void Scale(const glm::dvec3& scale);
    void LookAt(const glm::dvec3& target);
	void SetRotation(const glm::dvec3& eulerAngles);
	void SetScale(const glm::dvec3& scale);
	void SetMatrix(const glm::dmat4& newMatrix) { matrix = newMatrix; }

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

    bool decode(const YAML::Node& node) {

        Component::decode(node);

        if (!node["position"] || !node["rotation"] || !node["scale"])
            return false;
        /*----------Position-------------*/
        YAML::convert<glm::dvec3>::decode(node["position"], position);
        /*----------Scale-------------*/
        glm::dvec3 new_scale;
        YAML::convert<glm::dvec3>::decode(node["scale"], new_scale);
        SetScale(new_scale);
        /*----------Rotation-------------*/
        glm::dvec3 new_rotation;
        YAML::convert<glm::dvec3>::decode(node["rotation"], new_rotation);
        SetRotation(new_rotation);


        return true;
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