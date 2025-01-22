#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <memory>
#include <string>
#include <sstream>
#include "../MyGameEngine/Component.h"
#include <yaml-cpp/yaml.h>
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

YAML::Node Billboard::encode() {
    YAML::Node node;
    node["type"] = static_cast<int>(m_Type);

    // Using a YAML Node array for position and scale
    YAML::Node position;
    position.push_back(m_Position.x);
    position.push_back(m_Position.y);
    position.push_back(m_Position.z);
    node["position"] = position;

    YAML::Node scale;
    scale.push_back(m_Scale.x);
    scale.push_back(m_Scale.y);
    scale.push_back(m_Scale.z);
    node["scale"] = scale;

    return node;
}

bool Billboard::decode(const YAML::Node& node) {
    if (!node["type"] || !node["position"] || !node["scale"]) {
        return false;
    }

    // Read the type from the node
    m_Type = static_cast<BillboardType>(node["type"].as<int>());

    // Read the position and scale values as vectors
    m_Position = glm::vec3(node["position"][0].as<float>(),
        node["position"][1].as<float>(),
        node["position"][2].as<float>());

    m_Scale = glm::vec3(node["scale"][0].as<float>(),
        node["scale"][1].as<float>(),
        node["scale"][2].as<float>());

    return true;
}
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
