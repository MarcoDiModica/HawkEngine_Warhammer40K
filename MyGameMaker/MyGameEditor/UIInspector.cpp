#include "UIInspector.h"
#include "App.h"
#include "MyGUI.h"

#include "..\MyGameEngine\TransformComponent.h"
#include "..\MyGameEngine\CameraComponent.h"
#include "..\MyGameEngine\Mesh.h"
#include "EditorCamera.h"
#include "Input.h"
#include "..\MyGameEngine\types.h"
#include "..\MyGameEngine\MeshRendererComponent.h"
#include "..\MyGameEngine\Mesh.h"
#include "..\MyGameEngine\Image.h"
#include <string>

#include <imgui.h>
#include <imgui_internal.h>

UIInspector::UIInspector(UIType type, std::string name) : UIElement(type, name)
{
	matrixDirty = false;

	position = { 0, 0 ,0 };
	rotationRadians = { 0, 0, 0 };
	scale = { 1, 1, 1 };

}

UIInspector::~UIInspector()
{
}

bool UIInspector::Draw() {
    ImGuiWindowFlags inspectorFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoCollapse;
    ImGuiWindowClass windowClass;

    if (firstDraw) {
        ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
        firstDraw = false;
    }

    ImGui::SetNextWindowClass(&windowClass);
    windowClass.DockingAllowUnclassed = false;

    if (ImGui::Begin("Inspector", nullptr, inspectorFlags)) {
        ImGuiIO& io = ImGui::GetIO();

        if (Application->input->GetSelectedGameObjects().empty()) {
            ImGui::Text("No GameObject selected");
        }
        else {
            // Obtener el último GameObject seleccionado
            GameObject* selectedGameObject = Application->input->GetSelectedGameObjects().back();

            // Verificar si selectedGameObject es válido
            if (!selectedGameObject) {
                LOG(LogType::LOG_ERROR, "UIInspector::Draw: selectedGameObject is nullptr");
                ImGui::Text("Error: Invalid GameObject selected");
                return false;
            }

            char newName[128] = {};
            strncpy_s(newName, selectedGameObject->GetName().c_str(), sizeof(newName));
            ImGui::SameLine(); ImGui::Text("GameObject:");
            if (ImGui::InputText("##GameObjectName", newName, sizeof(newName), ImGuiInputTextFlags_EnterReturnsTrue)) {
                if (strlen(newName) > 0) {
                    selectedGameObject->SetName(newName);
                }
            }
            ImGui::SameLine(); ImGui::Checkbox("Static", &selectedGameObject->isStatic);

            Transform_Component* transform = selectedGameObject->GetTransform();

            if (transform) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Transform")) {
                    glm::dvec3 currentPosition = transform->GetPosition();
                    glm::dvec3 currentRotation = glm::radians(transform->GetEulerAngles());
                    glm::dvec3 currentScale = transform->GetScale();

                    float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
                    float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
                    float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

                    if (ImGui::DragFloat3("Postition", pos, 0.1f)) {
                        glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
                        glm::dvec3 deltaPos = newPosition - currentPosition;
                        transform->Translate(deltaPos);
                    }

                    if (ImGui::DragFloat3("Rotation", rot, 0.1f)) {
                        glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
                        glm::dvec3 deltaRot = newRotation - currentRotation;

                        transform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
                        transform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
                        transform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
                    }

                    if (ImGui::DragFloat3("Scale", sca, 0.1f, 0.1f, 10.0f)) {
                        glm::dvec3 newScale = { sca[0], sca[1], sca[2] };
                        glm::dvec3 deltaScale = newScale / currentScale;
                        transform->Scale(deltaScale);
                    }
                    ImGui::Checkbox("Snap", &snap);
                    ImGui::DragFloat("Snap Value", &snapValue, 0.1f, 0.1f, 10.0f);
                }
            }
            else {
                throw std::runtime_error("UIInspector::Draw: Transform component is nullptr");
            }

            ImGui::Separator();

            if (selectedGameObject->HasComponent<MeshRenderer>()) {
                MeshRenderer* meshRenderer = selectedGameObject->GetComponent<MeshRenderer>();

                // Verificar si meshRenderer es válido
                if (!meshRenderer) {
                    LOG(LogType::LOG_ERROR, "UIInspector::Draw: meshRenderer is nullptr");
                    ImGui::Text("Error: Invalid MeshRenderer component");
                    return false;
                }

                std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh();

                if (mesh) {
                    if (ImGui::CollapsingHeader("Mesh")) {
                        ImGui::Text("Vertices: %d", mesh->vertices().size());
                        ImGui::Text("Indices: %d", mesh->indices().size());

                        auto v = selectedGameObject->boundingBox().min;
                        ImGui::Text("BB min: %f", (float) v.x);


                        bool& triNormals = mesh->drawTriangleNormals;
                        bool& vertexNormals = mesh->drawVertexNormals;

                        ImGui::Checkbox("Tri Normals", &triNormals);
                        ImGui::Checkbox("Vertex Normals", &vertexNormals);
                    }
                }
                else {
                    LOG(LogType::LOG_WARNING, "UIInspector::Draw: MeshRenderer has no Mesh");
                }

                ImGui::Separator();

                std::shared_ptr<Image> image = meshRenderer->GetImage();

                if (image) {
                    if (ImGui::CollapsingHeader("Image")) {
                        ImGui::Text("Width: %d", image->width());
                        ImGui::Text("Heigth: %d", image->height());

                        ImGui::Separator();

                        auto textureID = image.get()->id();
                        if (textureID) {
                            ImGui::Text("Preview: %dx%d", image->width(), image->height());
                            ImVec2 imageSize = ImVec2(static_cast<float>(image->width()), static_cast<float>(image->height()));

                            float maxPreviewSize = 200.0f;
                            if (imageSize.x > maxPreviewSize || imageSize.y > maxPreviewSize) {
                                float aspectRatio = imageSize.x / imageSize.y;
                                if (aspectRatio > 1.0f) {
                                    imageSize.x = maxPreviewSize;
                                    imageSize.y = maxPreviewSize / aspectRatio;
                                }
                                else {
                                    imageSize.y = maxPreviewSize;
                                    imageSize.x = maxPreviewSize * aspectRatio;
                                }
                            }
                            ImGui::Image((void*)(intptr_t)textureID, imageSize);
                        }
                        else {
                            ImGui::Text("No texture loaded");
                        }
                    }
                }
                else {
                    LOG(LogType::LOG_WARNING, "UIInspector::Draw: MeshRenderer has no Image");
                }

                ImGui::Separator();

                if (selectedGameObject->HasComponent<CameraComponent>()) {
					CameraComponent* cameraComponent = selectedGameObject->GetComponent<CameraComponent>();

                    LOG(LogType::LOG_INFO, "UIInspector::Draw: CameraComponent found");

                    // Verificar si cameraComponent es válido
                    if (!cameraComponent) {
						LOG(LogType::LOG_ERROR, "UIInspector::Draw: CameraComponent is nullptr");
						ImGui::Text("Error: Invalid CameraComponent component");
						return false;
					}

					if (cameraComponent) {
						if (ImGui::CollapsingHeader("Camera")) {
							bool orthographic = cameraComponent->IsOrthographic();
							float orthoSize = cameraComponent->GetOrthoSize();
							float fov = cameraComponent->GetFOV();
							float nearPlane = cameraComponent->GetNearPlane();
							float farPlane = cameraComponent->GetFarPlane();

							if (ImGui::Checkbox("Orthographic", &orthographic)) {
								cameraComponent->SetOrthographic(orthographic, cameraComponent->GetNearPlane(), cameraComponent->GetFarPlane());
							}

							if (orthographic) {
								if (ImGui::DragFloat("Size", &orthoSize, 0.1f, 0.1f, 100.0f)) {
									cameraComponent->SetOrthoSize(orthoSize);
								}
							}
							else {
								if (ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 179.0f)) {
									cameraComponent->SetFOV(fov);
								}
							}

							if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.1f, 100.0f)) {
								cameraComponent->SetNearPlane(nearPlane);
							}

							if (ImGui::DragFloat("Far Plane", &farPlane, 0.1f, 0.1f, 1000.0f)) {
								cameraComponent->SetFarPlane(farPlane);
							}
						}
					}
					else {
						LOG(LogType::LOG_WARNING, "UIInspector::Draw: CameraComponent is nullptr");
					}
				}
            }
        }

        ImGui::End();
    }

    return true;
}