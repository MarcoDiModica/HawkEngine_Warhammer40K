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
    const auto& GetRotation() const { return rotation; }
    const auto& GetScale() const { return scale; }
    auto& GetPosition() { return position; }
    auto& GetRotation() { return rotation; }
    auto& GetScale() { return scale; }

    const auto* GetData() const { return &matrix[0][0]; }

    void Translate(const glm::dvec3& translation);
    void Rotate(double rads, const glm::dvec3& axis);
    void Scale(const glm::dvec3& scale);
    void LookAt(const glm::dvec3& target);

    //Transform_Component operator*(const mat4& other) { return Transform_Component(matrix * other); }
    //Transform_Component operator*(const Transform_Component& other) { return Transform_Component(matrix * other.matrix); }

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
            glm::dvec3 rotation; glm::dmat4::value_type rot_w;
            glm::dvec3 scale; glm::dmat4::value_type scale_w;
        };
    };
};

//inline Transform_Component operator*(const mat4& m, const Transform_Component& t) { return Transform_Component(m * t.GetMatrix()); }