#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <memory>
#include "../MyGameEngine/Component.h"

// Enum for the different types of billboards
enum class BillboardType
{
    SCREEN_ALIGNED,  // Aligned to the camera screen
    WORLD_ALIGNED,   // Aligned to the world
    AXIS_ALIGNED     // Aligned to a specific axis (typically the Y-axis)
};

class Billboard : public Component
{
public:
    // Constructor
    Billboard(GameObject* owner, BillboardType type, const glm::vec3& position, const glm::vec3& scale);

    // Override Component's methods
    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    ComponentType GetType() const override;
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    // Calculate the appropriate transformation matrix based on the billboard type
    glm::mat4 CalculateTransform(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);

    // Getter and Setter for BillboardType
    BillboardType GetTypeEnum() const;
    void SetType(BillboardType type);

    // Public getter and setter for m_Position
    const glm::vec3& GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec3& position) { m_Position = position; }

    // Public getter and setter for m_Scale
    const glm::vec3& GetScale() const { return m_Scale; }
    void SetScale(const glm::vec3& scale) { m_Scale = scale; }

    // Serialization - Converts the Billboard to a YAML node
    YAML::Node encode() override;
    // Deserialization - Decodes a YAML node into the Billboard's state
    bool decode(const YAML::Node& node) override;

private:
    // Transformation calculation for different types of billboards
    glm::mat4 CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
    glm::mat4 CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
    glm::mat4 CalculateAxisAligned(const glm::vec3& cameraPosition);

    // The type of the billboard (SCREEN_ALIGNED, etc.)
    BillboardType m_Type;
    // The position of the billboard in world space
    glm::vec3 m_Position;
    // The scale of the billboard in world space
    glm::vec3 m_Scale;
};