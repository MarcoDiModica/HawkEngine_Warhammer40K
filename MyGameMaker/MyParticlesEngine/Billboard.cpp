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
#include "../MyGameEngine/TransformComponent.h"


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


glm::mat4 Billboard::CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp)
{
    glm::vec3 N = -glm::normalize(glm::vec3(cameraPosition.x, 0, cameraPosition.z));
    glm::vec3 U = cameraUp;
    glm::vec3 R = glm::cross(U, N);

    glm::mat4 transform = glm::mat4(glm::vec4(R * m_Scale.x, 0.0f), glm::vec4(U * m_Scale.y, 0.0f), glm::vec4(N * m_Scale.z, 0.0f), glm::vec4(m_Position, 1.0f));

    return transform;
}

glm::mat4 Billboard::CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp)
{
    glm::vec3 N = glm::normalize(cameraPosition - m_Position);
    glm::vec3 U = cameraUp;
    glm::vec3 R = glm::normalize(glm::cross(U, N));
    U = glm::cross(N, R);

    glm::mat4 transform = glm::mat4(glm::vec4(R * m_Scale.x, 0.0f), glm::vec4(U * m_Scale.y, 0.0f), glm::vec4(N * m_Scale.z, 0.0f), glm::vec4(m_Position, 1.0f));

    return transform;
}

glm::mat4 Billboard::CalculateAxisAligned(const glm::vec3& cameraPosition)
{
    glm::vec3 dirToCamera = cameraPosition - m_Position;
    dirToCamera.y = 0.0f;
    dirToCamera = glm::normalize(dirToCamera);

    glm::vec3 R = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dirToCamera);

    glm::mat4 transform = glm::mat4(glm::vec4(R * m_Scale.x, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * m_Scale.y, glm::vec4(dirToCamera * m_Scale.z, 0.0f), glm::vec4(m_Position, 1.0f));

    return transform;
}