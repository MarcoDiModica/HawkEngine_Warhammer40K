#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

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
#include "..\MyGameEngine\Material.h"
#include "..\MyGameEngine\LightComponent.h"
#include <string>

#include <imgui.h>
#include <imgui_internal.h>
#include"../MyParticlesEngine/Billboard.h"
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

	if (!ImGui::Begin("Inspector", nullptr, inspectorFlags)) {
        ImGui::CloseCurrentPopup();
		ImGui::End();
		return false;
	}

    if (Application->input->GetSelectedGameObjects().empty()) {
        ImGui::Text("No GameObject selected");
    }
    else {
        // Obtener el �ltimo GameObject seleccionado
        GameObject* selectedGameObject = Application->input->GetSelectedGameObjects().back();

        // Verificar si selectedGameObject es v�lido
        if (!selectedGameObject) {
            LOG(LogType::LOG_ERROR, "UIInspector::Draw: selectedGameObject is nullptr");
            ImGui::Text("Error: Invalid GameObject selected");
            ImGui::End();
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

		ImGui::Separator();

        Transform_Component* transform = selectedGameObject->GetTransform();

        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentMenu");
        }

        if (ImGui::BeginPopup("AddComponentMenu")) {
            if (!selectedGameObject->HasComponent<CameraComponent>() && ImGui::MenuItem("Camera")) {
                selectedGameObject->AddComponent<CameraComponent>();
            }

            if (!selectedGameObject->HasComponent<MeshRenderer>() && ImGui::MenuItem("MeshRenderer")) {
                Application->root->AddMeshRenderer(*selectedGameObject, Mesh::CreateCube(), "Assets/default.png");
            }

            if (!selectedGameObject->HasComponent<LightComponent>() && ImGui::MenuItem("Light")) {
				selectedGameObject->AddComponent<LightComponent>();
			}
            if (!selectedGameObject->HasComponent<Billboard>() && ImGui::MenuItem("Billboard")) {
                selectedGameObject->AddComponent<Billboard>(BillboardType::SCREEN_ALIGNED, glm::vec3(0.0f), glm::vec3(1.0f));
            }
            // More components here

            ImGui::EndPopup();
        }

        ImGui::Separator();

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
        
        if (selectedGameObject->HasComponent<Billboard>()) {
            Billboard* billboard = selectedGameObject->GetComponent<Billboard>();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);


            if (ImGui::CollapsingHeader("Billboard")) {
                const char* billboardTypes[] = { "Screen Aligned", "World Aligned", "Axis Aligned" };
                int currentType = static_cast<int>(billboard->GetTypeEnum());
                if (ImGui::Combo("Type", &currentType, billboardTypes, IM_ARRAYSIZE(billboardTypes))) {
                    billboard->SetType(static_cast<BillboardType>(currentType));
                }

                glm::vec3 position = billboard->GetPosition();
                if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
                    billboard->SetPosition(position);
                }

                glm::vec3 scale = billboard->GetScale();
                if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
                    billboard->SetScale(scale);
                }
            }
        }

        ImGui::Separator();
        if (selectedGameObject->HasComponent<MeshRenderer>()) {
            MeshRenderer* meshRenderer = selectedGameObject->GetComponent<MeshRenderer>();

            // Verificar si meshRenderer es v�lido
            if (!meshRenderer) {
                LOG(LogType::LOG_ERROR, "UIInspector::Draw: meshRenderer is nullptr");
                ImGui::Text("Error: Invalid MeshRenderer component");
                return false;
            }

            std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh();

            if (mesh) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Mesh")) {
                    ImGui::Text("Vertices: %d", mesh->vertices().size());
                    ImGui::Text("Indices: %d", mesh->indices().size());

                    auto v = selectedGameObject->boundingBox().min;
                    ImGui::Text("BB min: %f", (float)v.x);


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

            std::shared_ptr<Image> image = meshRenderer->GetMaterial()->getImg();

            if (image) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
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
                        vec4 matColor = meshRenderer->GetMaterial()->GetColor();
                        float colorArray[4] = { static_cast<float>(matColor.x),static_cast<float>(matColor.y), static_cast<float>(matColor.z),static_cast<float>(matColor.w) };

                        if (ImGui::ColorPicker4("Color", colorArray))
                        {
                            vec4 retColor;
                            retColor.x = colorArray[0];
                            retColor.y = colorArray[1];
                            retColor.z = colorArray[2];
                            retColor.w = colorArray[3];

                            meshRenderer->GetMaterial()->SetColor(retColor);
                        }
                    }
                    else {
                        ImGui::Text("No texture loaded");
                    }
                }
            }
            else {
                LOG(LogType::LOG_WARNING, "UIInspector::Draw: MeshRenderer has no Image");
            }
        }

        ImGui::Separator();

        if (selectedGameObject->HasComponent<CameraComponent>()) {
			CameraComponent* cameraComponent = selectedGameObject->GetComponent<CameraComponent>();

            // Verificar si cameraComponent es v�lido
            if (!cameraComponent) {
				LOG(LogType::LOG_ERROR, "UIInspector::Draw: CameraComponent is nullptr");
				ImGui::Text("Error: Invalid CameraComponent component");
				return false;
			}

			if (cameraComponent) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (ImGui::CollapsingHeader("Camera")) {
					bool orthographic = cameraComponent->IsOrthographic();
                    bool frustum = cameraComponent->frustrumCullingEnabled;
					float orthoSize = cameraComponent->GetOrthoSize();
					float fov = cameraComponent->GetFOV();
					float nearPlane = static_cast<float>(cameraComponent->GetNearPlane());
					float farPlane =  static_cast<float>(cameraComponent->GetFarPlane() );

                    //lookAt & follow settings

					if (ImGui::Checkbox("Orthographic", &orthographic)) {
						cameraComponent->orthographic = orthographic;
					}

					if (orthographic) {
						if (ImGui::DragFloat("Size", &orthoSize, 0.1f, 0.1f, 100.0f)) {
							cameraComponent->SetOrthoSize(orthoSize);
						}
					}
					else {
                        float fovDeg = cameraComponent->GetFOV();
                        fovDeg = glm::degrees(fovDeg);
                        if (ImGui::SliderFloat("FOV", &fovDeg, 1.0f, 179.0f))
                        {
                            cameraComponent->SetFOV(glm::radians(fovDeg));
                        }
					}

					if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.1f, 100.0f)) {
						cameraComponent->SetNearPlane(nearPlane);
					}

					if (ImGui::DragFloat("Far Plane", &farPlane, 0.1f, 0.1f, 1000.0f)) {
						cameraComponent->SetFarPlane(farPlane);
					}

                    //frustrum settings
                    if (ImGui::Checkbox("Frustrum Culling", &frustum)) {
						cameraComponent->frustrumCullingEnabled = frustum;
					}

					if (frustum) {
						bool frustumRepresentation = cameraComponent->frustrumRepresentation;
						if (ImGui::Checkbox("Frustrum Representation", &frustumRepresentation)) {
							cameraComponent->frustrumRepresentation = frustumRepresentation;
						}
					}
				}
			}
			else {
				LOG(LogType::LOG_WARNING, "UIInspector::Draw: CameraComponent is nullptr");
			}
		}
        ImGui::Separator();

        if (selectedGameObject->HasComponent<LightComponent>())
        {
            LightComponent* lightComponent = selectedGameObject->GetComponent<LightComponent>();

            if (lightComponent)
            {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Light"))
                {
                    LightType lightType = lightComponent->GetLightType();
                    vec3 diffuse = lightComponent->GetDiffuse();
                    vec3 specular = lightComponent->GetSpecular();
                    vec3 ambient = lightComponent->GetAmbient();
                    float intensity = lightComponent->GetIntensity();
                    float radius = lightComponent->GetRadius();
                    float constant = lightComponent->GetConstant();
                    float linear = lightComponent->GetLinear();
                    float quadratic = lightComponent->GetQuadratic();

                    //glm::dvec3 direction = lightComponent->GetDirection();

                    if (ImGui::Combo("Type", (int*)&lightType, "Directional\0Point\0"))
                    {
                        lightComponent->SetLightType(lightType);
                    }

                    if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 100.0f))
                    {
                        lightComponent->SetIntensity(intensity);
                    }

                    /*if (ImGui::ColorEdit3("Color"))
                    {
                        lightComponent->SetColor(color); COLOR
                    }*/

                    if (lightType == LightType::POINT)
                    {
                        float diffusefloat[3] = { static_cast<float>(diffuse.x), static_cast<float>(diffuse.y), static_cast<float>(diffuse.z) };
                        float specularfloat[3] = { static_cast<float>(specular.x), static_cast<float>(specular.y), static_cast<float>(specular.z) };
                        float ambientfloat[3] = { static_cast<float>(ambient.x), static_cast<float>(ambient.y), static_cast<float>(ambient.z) };

                        if (ImGui::DragFloat("Range", &radius, 0.1f, 0.0f, 1000.0f))
                        {
                            lightComponent->SetRadius(radius);
                        }
                        if (ImGui::DragFloat("Constant", &constant, 0.1f, 0.0f, 1000.0f))
                        {
                            lightComponent->SetConstant(constant);
                        }
                        if (ImGui::DragFloat("Linear", &linear, 0.1f, 0.0f, 1000.0f))
                        {
                            lightComponent->SetLinear(linear);
                        }
                        if (ImGui::DragFloat("Quadratic", &quadratic, 0.1f, 0.0f, 1000.0f))
                        {
                            lightComponent->SetQuadratic(quadratic);
                        }
                        if (ImGui::ColorEdit3("Ambient", ambientfloat))
                        {
                            ambient = vec3(ambientfloat[0], ambientfloat[1], ambientfloat[2]);
                            lightComponent->SetAmbient(ambient);
                        }
                        if (ImGui::ColorEdit3("Diffuse", diffusefloat)) {
                            diffuse = vec3(diffusefloat[0], diffusefloat[1], diffusefloat[2]);
                            lightComponent->SetDiffuse(diffuse);
                        }
                        if (ImGui::ColorEdit3("Specular", specularfloat))
                        {
                            specular = vec3(specularfloat[0], specularfloat[1], specularfloat[2]);
                            lightComponent->SetSpecular(specular);
                        }
                    }

                    //if (lightType == LightType::DIRECTIONAL)
                    //{
                    //                   // change direction vec3
                    //}
                }
            }
            else
            {
                LOG(LogType::LOG_WARNING, "UIInspector::Draw: LightComponent is nullptr");
            }
        }
    }

    ImGui::End();
    return true;
}