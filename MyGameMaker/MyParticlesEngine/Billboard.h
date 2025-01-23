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
   


    glm::mat4 CalculateTransform(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);



    BillboardType GetTypeEnum() const;
    void SetType(BillboardType type);

    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetScale() const { return m_Scale; }

    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetScale(const glm::vec3& scale) { m_Scale = scale; }

    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
    glm::mat4 m_Transform;
private:
    // Transformation calculation for different types of billboards
    glm::mat4 CalculateScreenAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
        glm::mat4 CalculateWorldAligned(const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
    glm::mat4 CalculateAxisAligned(const glm::vec3& cameraPosition);
    // The type of the billboard (SCREEN_ALIGNED, etc.)
    BillboardType m_Type;
    // The position of the billboard in world space

 
};

//if (selectedGameObject->HasComponent<Billboard>()) {
//    Billboard* billboard = selectedGameObject->GetComponent<Billboard>();
//    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
//
//    if (ImGui::CollapsingHeader("Billboard")) {
//        const char* billboardTypes[] = { "Screen Aligned", "World Aligned", "Axis Aligned" };
//        int currentType = static_cast<int>(billboard->GetTypeEnum());
//        if (ImGui::Combo("Type", &currentType, billboardTypes, IM_ARRAYSIZE(billboardTypes))) {
//            billboard->SetType(static_cast<BillboardType>(currentType));
//        }
//
//        glm::vec3 position = billboard->GetPosition();
//        glm::vec3 scale = billboard->GetScale();
//        glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition();
//        glm::vec3 cameraUp = Application->camera->GetTransform().GetPosition();
//
//        if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
//            billboard->SetPosition(position);
//        }
//
//        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
//            billboard->SetScale(scale);
//        }
//
//        // Calculate and display the transformation matrix for reference
//        glm::mat4 transformMatrix = billboard->CalculateTransform(cameraPosition, cameraUp);
//        ImGui::Text("Transformation Matrix:");
//        for (int i = 0; i < 4; ++i) {
//            ImGui::Text("[%.2f, %.2f, %.2f, %.2f]", transformMatrix[i][0], transformMatrix[i][1], transformMatrix[i][2], transformMatrix[i][3]);
//        }
//    }
//}
//
//if (selectedGameObject->HasComponent<Billboard>()) {
//    Billboard* billboard = selectedGameObject->GetComponent<Billboard>();
//    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
//
//    if (ImGui::CollapsingHeader("Billboard")) {
//        const char* billboardTypes[] = { "Screen Aligned", "World Aligned", "Axis Aligned" };
//        int currentType = static_cast<int>(billboard->GetTypeEnum());
//        if (ImGui::Combo("Type", &currentType, billboardTypes, IM_ARRAYSIZE(billboardTypes))) {
//            billboard->SetType(static_cast<BillboardType>(currentType));
//        }
//
//        glm::vec3 position = billboard->GetPosition();
//        glm::vec3 scale = billboard->GetScale();
//
//        float billPos[3] = { position.x, position.y, position.z };
//        float billScale[3] = { scale.x, scale.y, scale.z };
//
//        if (ImGui::DragFloat3("Position", billPos, 0.1f)) {
//            glm::vec3 newBillPosition = { billPos[0], billPos[1], billPos[2] };
//            glm::vec3 deltaBillPos = newBillPosition - position;
//            transform->Translate(glm::dvec3(deltaBillPos));
//            billboard->SetPosition(newBillPosition);
//        }
//
//        if (ImGui::DragFloat3("Scale", billScale, 0.1f)) {
//            glm::vec3 newBillScale = { billScale[0], billScale[1], billScale[2] };
//            glm::vec3 deltaBillScale = newBillScale / scale;
//            transform->Scale(glm::dvec3(deltaBillScale));
//            billboard->SetScale(newBillScale);
//        }
//    }
//}