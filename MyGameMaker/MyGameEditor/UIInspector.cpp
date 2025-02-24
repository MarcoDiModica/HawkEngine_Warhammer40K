#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/reflection.h>
#include <Windows.h>

#include <imgui.h>
#include <imgui_internal.h>

#include "UIInspector.h"
#include "App.h"
#include "MyGUI.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/LightComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyAudioEngine/AudioListener.h"
#include "../MyGameEditor/Log.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/Mesh.h"
#include "Input.h"
#include "../MyGameEngine/types.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Material.h"
#include "../MyPhysicsEngine/ColliderComponent.h"
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "../MyScriptingEngine/ScriptComponent.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "../MyParticlesEngine/ParticlesEmitterComponent.h"


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

			if (!selectedGameObject->HasComponent<SoundComponent>() && ImGui::MenuItem("Sound")) {
				selectedGameObject->AddComponent<SoundComponent>();
			}

			if (!selectedGameObject->HasComponent<AudioListener>() && ImGui::MenuItem("Audio Listener")) {
				selectedGameObject->AddComponent<AudioListener>();
			}
            if (!selectedGameObject->HasComponent<ColliderComponent>() && ImGui::MenuItem("Collider")) {
                selectedGameObject->AddComponent<ColliderComponent>(Application->physicsModule);
                selectedGameObject->GetComponent<ColliderComponent>()->Start();
            }
            if (!selectedGameObject->HasComponent<RigidbodyComponent>() && ImGui::MenuItem("Rigidbody")) {
                selectedGameObject->AddComponent<RigidbodyComponent>(Application->physicsModule);
                selectedGameObject->GetComponent<RigidbodyComponent>()->Start();
            }
            
            if (!selectedGameObject->HasComponent<ParticlesEmitterComponent>() && ImGui::MenuItem("ParticleEmitter")) {
                selectedGameObject->AddComponent<ParticlesEmitterComponent>();
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

                    //Collider Rotation
                    if (selectedGameObject->HasComponent<ColliderComponent>()) {
                        ColliderComponent* collider = selectedGameObject->GetComponent<ColliderComponent>();
                        if (collider) {
                            glm::quat newRotation = glm::quat(glm::radians(glm::vec3(rot[0], rot[1], rot[2])));
                            collider->SetColliderRotation(newRotation);
                        }
                    }

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
                    ImGui::Text("Vertices: %d", mesh->getModel()->GetModelData().vertexData.size());
                    ImGui::Text("Indices: %d", mesh->getModel()->GetModelData().indexData.size());

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

        ImGui::Separator();


        if (selectedGameObject->HasComponent<ColliderComponent>()) {
            ColliderComponent* colliderComponent = selectedGameObject->GetComponent<ColliderComponent>();

            if (colliderComponent) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Collider")) {

                    glm::vec3 colliderPosition = colliderComponent->GetColliderPos();
                    float pos[3] = { colliderPosition.x, colliderPosition.y, colliderPosition.z };
                    if (ImGui::DragFloat3("Collider Position", pos, 0.1f)) {
                        colliderComponent->SetColliderPos(glm::vec3(pos[0], pos[1], pos[2]));
                    }

                    glm::quat colliderRotation = colliderComponent->GetColliderRotation();
                    glm::vec3 eulerRotation = glm::eulerAngles(colliderRotation);
                    float rot[3] = { glm::degrees(eulerRotation.x), glm::degrees(eulerRotation.y), glm::degrees(eulerRotation.z) };
                    if (ImGui::DragFloat3("Collider Rotation", rot, 0.1f)) {
                        glm::quat newRotation = glm::quat(glm::radians(glm::vec3(rot[0], rot[1], rot[2])));
                        colliderComponent->SetColliderRotation(newRotation);
                    }

                    bool snapToPosition = colliderComponent->GetSnapToPosition();
                    if (ImGui::Checkbox("Snap Position", &snapToPosition)) {
                        colliderComponent->SetSnapToPosition(snapToPosition);
                    }

                    bool resetRotation = false;
                    if (ImGui::Checkbox("Reset Rotation", &resetRotation)) {
                        if (resetRotation) {
                            if (selectedGameObject->HasComponent<ColliderComponent>()) {
                                ColliderComponent* collider = selectedGameObject->GetComponent<ColliderComponent>();
                                if (collider) {
                                    glm::quat newRotation = glm::quat(glm::radians(glm::vec3(0, 0, 0)));
                                    collider->SetColliderRotation(newRotation);
                                }
                            }
                        }
                    }

                    bool isTrigger = colliderComponent->IsTrigger();
                    if (ImGui::Checkbox("Is Trigger", &isTrigger)) {
                        colliderComponent->SetTrigger(isTrigger);
                    }

                    glm::vec3 size = colliderComponent->GetSize();
                    float sizeArray[3] = { size.x, size.y, size.z };
                    if (ImGui::DragFloat3("Collider Size", sizeArray, 0.1f, 0.1f, 100.0f)) {
                        colliderComponent->SetSize(glm::vec3(sizeArray[0], sizeArray[1], sizeArray[2]));
                    }
                }
            }
        }
        ImGui::Separator();

        // Draw RigidbodyComponent
        if (selectedGameObject->HasComponent<RigidbodyComponent>()) {
            RigidbodyComponent* rigidbodyComponent = selectedGameObject->GetComponent<RigidbodyComponent>();

            if (rigidbodyComponent) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Rigidbody")) {

                    float mass = rigidbodyComponent->GetMass();
                    if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.1f, 10.0f)) {
                        rigidbodyComponent->SetMass(mass);
                    }

                    bool freezeRotation = rigidbodyComponent->IsFreezed();
                    if (ImGui::Checkbox("Freeze Rotation", &freezeRotation)) {
                        rigidbodyComponent->SetFreezeRotations(freezeRotation);
                        
                    }

                    float friction = rigidbodyComponent->GetFriction();
                    if (ImGui::DragFloat("Friction", &friction, 0.1f, 0.0f, 10.0f)) {
                        rigidbodyComponent->SetFriction(friction);
                    }

                    float damping[2] = { rigidbodyComponent->GetDamping().x, rigidbodyComponent->GetDamping().y };
                    if (ImGui::DragFloat2("Damping (Linear, Angular)", damping, 0.1f, 0.0f, 10.0f)) {
                        rigidbodyComponent->SetDamping(damping[0], damping[1]);
                    }

                    bool isKinematic = rigidbodyComponent->IsKinematic();
                    if (ImGui::Checkbox("Is Kinematic", &isKinematic)) {
                        rigidbodyComponent->SetKinematic(isKinematic);
                    }
                    glm::vec3 gravity = rigidbodyComponent->GetGravity();  
                    if (ImGui::DragFloat3("Gravity", &gravity[0], 0.1f)) {  
                        rigidbodyComponent->SetGravity(gravity);  
                    }
                }
            }
        }

        ImGui::Separator();

        if (selectedGameObject->HasComponent<ParticlesEmitterComponent>())
        {
            DrawParticleEmitterInspector(selectedGameObject);
        }

        ImGui::Separator();

        if (selectedGameObject->HasComponent<SoundComponent>()) {
            SoundComponent* soundComponent = selectedGameObject->GetComponent<SoundComponent>();

            if (soundComponent) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Sound")) {
                    // Audio File Path
                    char audioPath[256];
                    strcpy_s(audioPath, soundComponent->GetAudioPath().c_str());
                    if (ImGui::InputText("Audio File", audioPath, sizeof(audioPath))) {
                        soundComponent->LoadAudio(audioPath);
                    }

                    // Drag and drop target for audio files
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                            const char* path = (const char*)payload->Data;
                            std::string extension = std::filesystem::path(path).extension().string();
                            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                            
                            if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") {
                                soundComponent->LoadAudio(path);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // Audio Type
                    bool isMusic = soundComponent->IsMusic();
                    if (ImGui::Checkbox("Is Music", &isMusic)) {
                        if (soundComponent->GetAudioPath().length() > 0) {
                            soundComponent->LoadAudio(soundComponent->GetAudioPath(), isMusic);
                        }
                    }

                    // Spatial Audio
                    bool isSpatial = soundComponent->IsSpatial();
                    if (ImGui::Checkbox("3D Sound", &isSpatial)) {
                        soundComponent->SetSpatial(isSpatial);
                    }

                    // Volume Control
                    float volume = soundComponent->GetVolume();
                    if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
                        soundComponent->SetVolume(volume);
                    }

                    // Loop Setting
                    bool loop = soundComponent->GetLoop();
                    if (ImGui::Checkbox("Loop", &loop)) {
                        soundComponent->SetLoop(loop);
                    }

                    // Auto-play setting
                    bool autoPlay = soundComponent->GetAutoPlay();
                    if (ImGui::Checkbox("Auto Play", &autoPlay)) {
                        soundComponent->SetAutoPlay(autoPlay);
                    }

                    ImGui::Separator();

                    // Playback Controls
                    if (soundComponent->IsPlaying()) {
                        if (ImGui::Button("Stop")) {
                            soundComponent->Stop();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Pause")) {
                            soundComponent->Pause();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Resume")) {
                            soundComponent->Resume();
                        }
                    } else {
                        if (ImGui::Button("Play")) {
                            soundComponent->Play(soundComponent->GetLoop());
                        }
                    }

                    if (isSpatial) {
                        ImGui::Text("Position is controlled by Transform component");
                    }
                }
            }
        }

        ImGui::Separator();
        if (selectedGameObject->HasComponent<AudioListener>())
        {
            AudioListener* audioListener = selectedGameObject->GetComponent<AudioListener>();

            if (audioListener)
            if (selectedGameObject->HasComponent<AudioListener>())
            {
                AudioListener* audioListener = selectedGameObject->GetComponent<AudioListener>();

                if (audioListener)
                {
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                    if (ImGui::CollapsingHeader("Audio Listener"))
                    {
                        ImGui::Text("Audio Listener");

                        // Mostrar la posición del Audio Listener
                        Transform_Component* transform = selectedGameObject->GetTransform();
                        if (transform)
                        {
                            glm::dvec3 position = transform->GetPosition();
                            float pos[3] = { static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z) };
                            ImGui::DragFloat3("Position", pos, 0.1f);
                        }
                        else
                        {
                            ImGui::Text("Error: Transform component is nullptr");
                        }
                    }
                }
                else
                {
                    LOG(LogType::LOG_WARNING, "UIInspector::Draw: AudioListener is nullptr");
                }
            }
            
               
        }
        
        ImGui::Separator();
      
        if (selectedGameObject->scriptComponents.size() > 0) {
            for (auto& scriptComponent : selectedGameObject->scriptComponents) {
                if (scriptComponent && scriptComponent->monoScript) {
                    ImGui::PushID(scriptComponent.get());
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                    if (ImGui::CollapsingHeader(scriptComponent->GetName().c_str()))
                    {
                        std::string scriptName = mono_class_get_name(mono_object_get_class(scriptComponent->monoScript));
                        ImGui::Text("Script: %s", scriptName.c_str());

                        if (ImGui::Button("Open Script")) {
                            std::string scriptPath = std::filesystem::absolute("../Script/" + scriptName + ".cs").string();
                            LOG(LogType::LOG_INFO, "Absolute script path: %s", scriptPath.c_str());

                            if (!std::filesystem::exists(scriptPath)) {
                                LOG(LogType::LOG_ERROR, "Script file does not exist at path: %s", scriptPath.c_str());
                            }
                            else {
                                HINSTANCE result = ShellExecuteA(NULL, "open", scriptPath.c_str(), NULL, NULL, SW_SHOW);
                                if ((int)result <= 32) {
                                    LOG(LogType::LOG_ERROR, "Failed to open script file: %s. Error code: %d", scriptPath.c_str(), (int)result);
                                }
                                else {
                                    LOG(LogType::LOG_INFO, "Successfully opened script: %s", scriptPath.c_str());
                                }
                            }
                        }


                        if (ImGui::Button("Reload Script")) {
                            if (!scriptComponent->LoadScript(scriptName)) {
                                LOG(LogType::LOG_ERROR, "Failed to reload script %s.", scriptName.c_str());
                            }
                            else {
                                LOG(LogType::LOG_INFO, "Script %s reloaded successfully.", scriptName.c_str());
                            }
                        }

                        MonoClass* scriptClass = mono_object_get_class(scriptComponent->monoScript);

                        MonoClassField* field = nullptr;
                        void* iter = nullptr;
                        while ((field = mono_class_get_fields(scriptClass, &iter))) {
                            const char* fieldName = mono_field_get_name(field);
                            MonoType* fieldType = mono_field_get_type(field);
                            int typeCode = mono_type_get_type(fieldType);

                            MonoClass* fieldParentClass = mono_field_get_parent(field);

                            ImGui::PushID(field);

                            if (typeCode == MONO_TYPE_STRING) {
                                MonoString* monoString = nullptr;
                                mono_field_get_value(scriptComponent->monoScript, field, &monoString);
                                std::string value = monoString ? mono_string_to_utf8(monoString) : "";

                                char buffer[128];
                                strncpy(buffer, value.c_str(), sizeof(buffer));
                                buffer[sizeof(buffer) - 1] = '\0';

                                if (ImGui::InputText(fieldName, buffer, sizeof(buffer))) {
                                    value = buffer;
                                    MonoString* newMonoString = mono_string_new(mono_domain_get(), value.c_str());
                                    mono_field_set_value(scriptComponent->monoScript, field, newMonoString);
                                }
                            }
                            else if (typeCode == MONO_TYPE_BOOLEAN) {
                                bool value = false;
                                mono_field_get_value(scriptComponent->monoScript, field, &value);
                                if (ImGui::Checkbox(fieldName, &value)) {
                                    mono_field_set_value(scriptComponent->monoScript, field, &value);
                                }
                            }
                            else if (typeCode == MONO_TYPE_I4) {
                                int value = 0;
                                mono_field_get_value(scriptComponent->monoScript, field, &value);
                                if (ImGui::InputInt(fieldName, &value)) {
                                    mono_field_set_value(scriptComponent->monoScript, field, &value);
                                }
                            }
                            else if (typeCode == MONO_TYPE_R4) {
                                float value = 0.0f;
                                mono_field_get_value(scriptComponent->monoScript, field, &value);
                                if (ImGui::InputFloat(fieldName, &value)) {
                                    mono_field_set_value(scriptComponent->monoScript, field, &value);
                                }
                            }
                            else if (typeCode == MONO_TYPE_R8) {
                                double value = 0.0;
                                mono_field_get_value(scriptComponent->monoScript, field, &value);
                                if (ImGui::InputDouble(fieldName, &value)) {
                                    mono_field_set_value(scriptComponent->monoScript, field, &value);
                                }
                            }

                            ImGui::PopID();
                        }
                    }
                    ImGui::PopID();
                }
                else {
                    if (!scriptComponent) {
                        LOG(LogType::LOG_WARNING, "UIInspector::Draw: ScriptComponent is nullptr");
                    }
                    else {
                        LOG(LogType::LOG_WARNING, "UIInspector::Draw: scriptComponent->monoScript is nullptr");
                    }
                }
            }

        }

        ImGui::Separator();

        if (selectedGameObject->HasComponent<ShaderComponent>()) {
            ShaderComponent* shaderComponent = selectedGameObject->GetComponent<ShaderComponent>();

            if (shaderComponent) {

                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Shader")) {
                    ShaderType currentType = shaderComponent->GetShaderType();
                    Shaders* currentShader = shaderComponent->GetShader();

                    // Mostrar y editar el tipo de shader
                    int shaderType = static_cast<int>(currentType);
                    if (ImGui::Combo("Shader Type", &shaderType, "Default\0Light\0Water\0\0")) {
                        shaderComponent->SetShaderType(static_cast<ShaderType>(shaderType));
                    }

                    if (currentType == ShaderType::WATER) {;
                        if (ImGui::DragFloat("Amplitude", &shaderComponent->amplitude, 0.1f, 0.1f, 10.0f));
                        if (ImGui::DragFloat("Frequency", &shaderComponent->frequency, 0.1f, 0.1f, 10.0f));
                    }
                }
            }
        }

    }

    ImGui::End();
    return true;
}

void UIInspector::DrawParticleEmitterInspector(GameObject* selectedGameObject) {
    if (!selectedGameObject || !selectedGameObject->HasComponent<ParticlesEmitterComponent>()) {
        return;
    }

    auto* particlesEmitter = selectedGameObject->GetComponent<ParticlesEmitterComponent>();
    if (!particlesEmitter) {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    bool isOpen = ImGui::CollapsingHeader("Particle Emitter", ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::PopStyleVar();

    if (!isOpen) {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
    ImGui::Indent(10.0f);

    if (ImGui::TreeNodeEx("Emission Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        float spawnRate = particlesEmitter->getSpawnRate();
        if (ImGui::DragFloat("Spawn Rate##spawner", &spawnRate, 0.1f, 0.1f, 100.0f, "%.1f")) {
            particlesEmitter->setSpawnRate(spawnRate);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Number of particles spawned per second");
        }

        int maxParticles = particlesEmitter->getMaxParticles();
        if (ImGui::DragInt("Max Particles##count", &maxParticles, 1, 1, 1000)) {
            particlesEmitter->setMaxParticles(maxParticles);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Maximum number of particles allowed at once");
        }
        ImGui::EndGroup();

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Rendering Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        BillboardType billboardType = particlesEmitter->GetTypeEnum();
        const char* billboardItems[] = {
            "Screen Aligned - Always faces camera",
            "World Aligned - Maintains up vector",
            "Axis Aligned - Rotates around specified axis"
        };

        if (ImGui::Combo("Billboard Type##alignment", (int*)&billboardType, billboardItems, IM_ARRAYSIZE(billboardItems))) {
            particlesEmitter->SetType(billboardType);
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Texture");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted("Supported formats: .png, .jpg, .jpeg, .bmp");
            ImGui::TextUnformatted("Drag and drop a texture file here");
            ImGui::EndTooltip();
        }

        std::string texturePath = particlesEmitter->GetTexture();
        char texturePathBuffer[256];
        strncpy_s(texturePathBuffer, sizeof(texturePathBuffer), texturePath.c_str(), _TRUNCATE);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        bool textureChanged = ImGui::InputText("##TexturePath", texturePathBuffer, sizeof(texturePathBuffer));
        ImGui::PopStyleVar();

        if (!texturePath.empty()) {
            ImGui::SameLine();
            if (ImGui::Button("Preview##texture")) {
            }
        }

        if (textureChanged) {
            particlesEmitter->SetTexture(texturePathBuffer);
        }

        ImGui::Button("Drop Texture Here", ImVec2(ImGui::GetContentRegionAvail().x, 30));
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                const char* path = static_cast<const char*>(payload->Data);
                std::string extension = std::filesystem::path(path).extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                const std::array<std::string, 5> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".image"};
                if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
                    particlesEmitter->SetTexture(path);
                }
                else {
                    LOG(LogType::LOG_WARNING, "Invalid texture format: %s", extension.c_str());
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::EndGroup();
        ImGui::TreePop();
    }

    ImGui::Unindent(10.0f);
    ImGui::PopStyleVar();
}