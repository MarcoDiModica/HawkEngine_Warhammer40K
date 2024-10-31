#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"

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