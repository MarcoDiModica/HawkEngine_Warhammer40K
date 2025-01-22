#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <memory>
#include <string>
#include <sstream>
#include "../MyGameEngine/Component.h"
#include "Billboard.h"

Billboard::Billboard(GameObject* owner, BillboardType type, const glm::vec3& position, const glm::vec3& scale)
    : Component(owner), m_Type(type), m_Position(position), m_Scale(scale) {}

// Override the Start, Update, Destroy methods from Component
void Billboard::Start() {
    // Initialize or set up resources
}

void Billboard::Update(float deltaTime) {
    // Perform per-frame updates (if any)
}

void Billboard::Destroy() {
    // Cleanup before destruction
}

// Override GetType to return the component type
ComponentType Billboard::GetType() const {
    return ComponentType::BILLBOARD; // Assuming BILLBOARD is added to ComponentType enum
}

// Override Clone to create a new instance of Billboard with a new owner
std::unique_ptr<Component> Billboard::Clone(GameObject* new_owner) {
    return std::make_unique<Billboard>(new_owner, m_Type, m_Position, m_Scale);
}

// Calculate the appropriate transformation matrix
glm::mat4 Billboard::CalculateTransform(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    switch (m_Type) {
    case BillboardType::SCREEN_ALIGNED:
        return CalculateScreenAligned(cameraPosition, cameraUp);
    case BillboardType::WORLD_ALIGNED:
        return CalculateWorldAligned(cameraPosition, cameraUp);
    case BillboardType::AXIS_ALIGNED:
        return CalculateAxisAligned(cameraPosition);
    default:
        return glm::mat4(1.0f);
    }
}

BillboardType Billboard::GetTypeEnum() const { return m_Type; }
void Billboard::SetType(BillboardType type) { m_Type = type; }

glm::mat4 Billboard::CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    glm::vec3 lookDirection = glm::normalize(cameraPosition - m_Position);
    glm::vec3 right = glm::normalize(glm::cross(cameraUp, lookDirection));

    // Ensure right is not a zero vector
    if (glm::length(right) < glm::epsilon<float>()) {
        right = glm::vec3(1.0f, 0.0f, 0.0f); // Use a default value if cross product is near zero
    }

    glm::vec3 up = glm::cross(lookDirection, right);

    glm::mat4 result;
    result[0] = glm::vec4(right * m_Scale.x, 0.0f);    // Right vector as a column
    result[1] = glm::vec4(up * m_Scale.y, 0.0f);       // Up vector as a column
    result[2] = glm::vec4(-lookDirection * m_Scale.z, 0.0f); // Negative lookDirection as a column
    result[3] = glm::vec4(m_Position, 1.0f);           // Position as the translation column

    return result;
}

glm::mat4 Billboard::CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    glm::vec3 lookDirection = glm::normalize(cameraPosition - m_Position);
    glm::vec3 right = glm::normalize(glm::cross(cameraUp, lookDirection));

    // Ensure right is not a zero vector
    if (glm::length(right) < glm::epsilon<float>()) {
        right = glm::vec3(1.0f, 0.0f, 0.0f); // Use a default value if cross product is near zero
    }

    glm::vec3 up = glm::cross(lookDirection, right);

    glm::mat4 result;
    result[0] = glm::vec4(right * m_Scale.x, 0.0f);    // Right vector as a column
    result[1] = glm::vec4(up * m_Scale.y, 0.0f);       // Up vector as a column
    result[2] = glm::vec4(lookDirection * m_Scale.z, 0.0f); // LookDirection as a column
    result[3] = glm::vec4(m_Position, 1.0f);           // Position as the translation column

    return result;
}

glm::mat4 Billboard::CalculateAxisAligned(const glm::vec3& cameraPosition) {
    glm::vec3 lookDirection = cameraPosition - m_Position;
    lookDirection.y = 0.0f; // Constraint to Y-axis for ground plane

    // Ensure the lookDirection is not a zero vector
    if (glm::length(lookDirection) < glm::epsilon<float>()) {
        lookDirection = glm::vec3(1.0f, 0.0f, 0.0f); // Handle case where the look direction is too small
    }

    lookDirection = glm::normalize(lookDirection);

    glm::vec3 right = glm::vec3(lookDirection.z, 0.0f, -lookDirection.x); // Perpendicular in XZ-plane

    glm::mat4 result;
    result[0] = glm::vec4(right * m_Scale.x, 0.0f);    // Right vector as a column
    result[1] = glm::vec4(0.0f, m_Scale.y, 0.0f, 0.0f); // Y-axis as a column
    result[2] = glm::vec4(lookDirection * m_Scale.z, 0.0f); // Look direction as a column
    result[3] = glm::vec4(m_Position, 1.0f);             // Position as the translation column

    return result;
}
