#pragma region Includes
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/class.h> 
#include <mono/metadata/reflection.h>
#include <Windows.h>

#include <imgui.h>
#include <imgui_internal.h>

#include "UIInspector.h"
#include "App.h"
#include "MyGUI.h"
#include "Input.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/LightComponent.h"
#include "../MyAudioEngine/SoundComponent.h"
#include "../MyAudioEngine/AudioListener.h"
#include "../MyGameEditor/Log.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/types.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/Image.h"
#include "../MyGameEngine/Material.h"
#include "../MyPhysicsEngine/ColliderComponent.h"
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "../MyPhysicsEngine/MeshColliderComponent.h"
#include "../MyScriptingEngine/ScriptComponent.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"

#include <Windows.h>
#include "../MyParticlesEngine/ParticleFX.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UITransformComponent.h"

typedef unsigned int guint32;
#pragma endregion

class ComponentDrawer {
private:
    #pragma region Transform
    static void DrawTransformComponent(Transform_Component* transform, bool& snap, float& snapValue) {
        if (!transform) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Transform")) return;

        if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Reset Transform")) {
				transform->ResetTransform();
			}
			ImGui::EndPopup();
		}

        GameObject* parent = transform->GetOwner()->GetParent();
        bool hasParent = parent != nullptr;
        ImGui::Text("Parent: %s", hasParent ? parent->GetName().c_str() : "None");

        glm::dvec3 currentPosition = transform->GetPosition();
        glm::dvec3 currentRotation = glm::radians(transform->GetEulerAngles());
        glm::dvec3 currentScale = transform->GetScale();

        float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
        float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
        float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

        if (ImGui::DragFloat3("Position", pos, 0.1f)) {
            glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
            transform->SetPosition(newPosition);
        }

        if (ImGui::DragFloat3("Rotation", rot, 0.1f)) {
            glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
            transform->SetRotation(newRotation);
        }
        if (ImGui::DragFloat3("Scale", sca, 0.01f, 0.01f, 100.0f)) {
            glm::dvec3 newScale = { sca[0], sca[1], sca[2] };
            transform->SetScale(newScale);
        }

        ImGui::Checkbox("Snap", &snap);
        ImGui::DragFloat("Snap Value", &snapValue, 0.1f, 0.1f, 10.0f);
    }
    #pragma endregion

    #pragma region MeshRenderer
	static void DrawMeshProperties(std::shared_ptr<Mesh> mesh) {
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Mesh")) return;

		ImGui::Text("Vertices: %d", mesh->getModel()->GetModelData().vertexData.size());
		ImGui::Text("Indices: %d", mesh->getModel()->GetModelData().indexData.size());
	}

	static void DrawTexturePreview(std::shared_ptr<Image> image, const char* label) {
		if (!image || image->id() == 0) return;

		ImGui::PushID(label);
		if (ImGui::Button("Show Preview")) {
			ImGui::OpenPopup("TexturePreview");
		}

		ImGui::SameLine();
		ImGui::Text("%s: %dx%d", label, image->width(), image->height());

		if (ImGui::BeginPopup("TexturePreview")) {
			ImVec2 imageSize = CalculatePreviewSize(image->width(), image->height());
			ImGui::Image((void*)(intptr_t)image->id(), imageSize);
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	static void DrawColorPicker(MeshRenderer* meshRenderer) {
		vec4 matColor = meshRenderer->GetMaterial()->GetColor();
		float colorArray[4] = {
			static_cast<float>(matColor.x),
			static_cast<float>(matColor.y),
			static_cast<float>(matColor.z),
			static_cast<float>(matColor.w)
		};

		if (ImGui::ColorEdit4("Color", colorArray)) {
			vec4 newColor(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
			meshRenderer->GetMaterial()->SetColor(newColor);
		}
	}

	static void DrawMaterialProperties(MeshRenderer* meshRenderer) {
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Material")) return;

		auto material = meshRenderer->GetMaterial();
		if (!material) return;

		if (ImGui::TreeNodeEx("Base Color", ImGuiTreeNodeFlags_DefaultOpen)) {
			DrawColorPicker(meshRenderer);

			std::shared_ptr<Image> image = material->imagePtr;
			if (image) {
				DrawTexturePreview(image, "Albedo");
			}

			if (ImGui::Button("Load Albedo Texture")) {
				ImGui::OpenPopup("LoadAlbedoTexture");
			}
			if (ImGui::BeginPopup("LoadAlbedoTexture")) {
				ImGui::Text("Drag and drop an albedo/diffuse texture file here");
				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
					const char* path = static_cast<const char*>(payload->Data);
					std::string extension = std::filesystem::path(path).extension().string();
					std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

					const std::array<std::string, 5> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".tga" };
					if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
						auto newImage = std::make_shared<Image>();
						if (newImage->LoadTexture(path)) {
							meshRenderer->SetImage(newImage);
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::TreePop();
		}

		if (material->GetShaderType() == ShaderType::PBR) {
			if (ImGui::TreeNodeEx("Normal Map", ImGuiTreeNodeFlags_DefaultOpen)) {
				std::shared_ptr<Image> normalMap = material->normalMapPtr;
				if (normalMap) {
					DrawTexturePreview(normalMap, "Normal");
				}
				else {
					ImGui::Text("No normal map assigned");
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Metallic", ImGuiTreeNodeFlags_DefaultOpen)) {
				std::shared_ptr<Image> metallicMap = material->metallicMapPtr;
				if (metallicMap) {
					DrawTexturePreview(metallicMap, "Metallic");
				}
				else {
					float metallic = material->metallic;
					if (ImGui::SliderFloat("Metallic Value", &metallic, 0.0f, 1.0f)) {
						material->metallic = metallic;
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Roughness", ImGuiTreeNodeFlags_DefaultOpen)) {
				std::shared_ptr<Image> roughnessMap = material->roughnessMapPtr;
				if (roughnessMap) {
					DrawTexturePreview(roughnessMap, "Roughness");
				}
				else {
					float roughness = material->roughness;
					if (ImGui::SliderFloat("Roughness Value", &roughness, 0.0f, 1.0f)) {
						material->roughness = roughness;
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Ambient Occlusion", ImGuiTreeNodeFlags_DefaultOpen)) {
				std::shared_ptr<Image> aoMap = material->aoMapPtr;
				if (aoMap) {
					DrawTexturePreview(aoMap, "AO");
				}
				else {
					float ao = material->ao;
					if (ImGui::SliderFloat("AO Value", &ao, 0.0f, 1.0f)) {
						material->ao = ao;
					}
				}
				ImGui::TreePop();
			}

            if (ImGui::TreeNodeEx("Tonemap", ImGuiTreeNodeFlags_DefaultOpen)) {
				float tonemapStrength = material->GetTonemapStrength();
				if (ImGui::SliderFloat("Tonemap Strength", &tonemapStrength, 0.0f, 10.0f)) {
					material->SetTonemapStrength(tonemapStrength);
				}
				ImGui::TreePop();
			}
		}
	}

	static void DrawMeshRendererComponent(MeshRenderer* meshRenderer) {
		if (!meshRenderer) return;

		std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh();
		if (mesh) {
			DrawMeshProperties(mesh);
		}

		DrawMaterialProperties(meshRenderer);
	}
    #pragma endregion

    #pragma region Camera
    static void DrawCameraComponent(CameraComponent* camera) {
        if (!camera) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Camera")) return;

        if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				camera->GetOwner()->RemoveComponent<CameraComponent>();
			}
			ImGui::EndPopup();
		}

        bool orthographic = camera->IsOrthographic();
        bool frustum = camera->frustrumCullingEnabled;
        float orthoSize = camera->GetOrthoSize();
        float fov = camera->GetFOV();
        auto nearPlane = static_cast<float>(camera->GetNearPlane());
        auto farPlane = static_cast<float>(camera->GetFarPlane());

        if (ImGui::Checkbox("Orthographic", &orthographic)) {
            camera->orthographic = orthographic;
        }

        if (orthographic) {
            if (ImGui::DragFloat("Size", &orthoSize, 0.1f, 0.1f, 100.0f)) {
                camera->SetOrthoSize(orthoSize);
            }
        }
        else {
            float fovDeg = glm::degrees(camera->GetFOV());
            if (ImGui::SliderFloat("FOV", &fovDeg, 1.0f, 179.0f)) {
                camera->SetFOV(glm::radians(fovDeg));
            }
        }

        if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.1f, 100.0f)) {
            camera->SetNearPlane(nearPlane);
        }

        if (ImGui::DragFloat("Far Plane", &farPlane, 0.1f, 0.1f, 1000.0f)) {
            camera->SetFarPlane(farPlane);
        }

        if (ImGui::Checkbox("Frustum Culling", &frustum)) {
            camera->frustrumCullingEnabled = frustum;
        }

        if (frustum) {
            bool frustumRepresentation = camera->frustrumRepresentation;
            ImGui::Checkbox("Frustum Representation", &frustumRepresentation);
            camera->frustrumRepresentation = frustumRepresentation;
        }

        int priority = camera->GetPriority();
        if (ImGui::DragInt("Priority", &priority, 0.1f, 1.0f, 10.0f)) {
            camera->SetPriority(priority);
			Application->root->UpdateCameraPriority();
        }

        //follow target look target and shake config
        GameObject* followTarget = camera->followTarget;
        if (followTarget) {
			ImGui::Text("Follow Target: %s", followTarget->GetName().c_str());

            float followDistance = static_cast<float>(camera->GetDistance());
            glm::dvec3 followOffset = camera->GetOffset();
            float followSmoothness = static_cast<float>(camera->followSmoothness);
            bool followX = camera->followX;
            bool followY = camera->followY;
            bool followZ = camera->followZ;

            if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
					GameObject* target = static_cast<GameObject*>(payload->Data);
					camera->followTarget = target;
				}
				ImGui::EndDragDropTarget();
			}

            if (ImGui::DragFloat("Distance", &followDistance, 0.1f, 0.1f, 100.0f)) {
				camera->SetDistance(followDistance);
			}

            float offset[3] = { followOffset.x, followOffset.y, followOffset.z };
			if (ImGui::DragFloat3("Offset", offset, 0.1f)) {
				camera->SetOffset(glm::dvec3(offset[0], offset[1], offset[2]));
			}

			if (ImGui::DragFloat("Smoothness", &followSmoothness, 0.01f, 0.01f, 1.0f)) {
				camera->followSmoothness = followSmoothness;
			}

			if (ImGui::Checkbox("Follow X", &followX)) {
				camera->followX = followX;
			}
			if (ImGui::Checkbox("Follow Y", &followY)) {
				camera->followY = followY;
			}
			if (ImGui::Checkbox("Follow Z", &followZ)) {
				camera->followZ = followZ;
			}

            if (ImGui::Button("Clear Follow Target")) {
				camera->followTarget = nullptr;
			}
		}

        //shake config
        float shakeIntensity = camera->shakeIntensity;
        float shakeDuration = camera->shakeDuration;
        float shakeFrequency = camera->shakeFrequency;

        if (ImGui::DragFloat("Shake Intensity", &shakeIntensity, 0.1f, 0.0f, 10.0f)) {
            camera->shakeIntensity = shakeIntensity;
        }

        if (ImGui::DragFloat("Shake Duration", &shakeDuration, 0.1f, 0.0f, 10.0f)) {
			camera->shakeDuration = shakeDuration;
		}

        if (ImGui::DragFloat("Shake Frequency", &shakeFrequency, 0.1f, 0.0f, 10.0f)) {
            camera->shakeFrequency = shakeFrequency;
        }

        if (ImGui::Button("Shake")) {
            camera->Shake(shakeIntensity, shakeDuration, shakeFrequency);
        }
    }
    #pragma endregion

    #pragma region Light
    static void DrawLightComponent(LightComponent* light) {
        if (!light) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Light")) return;

        LightType lightType = light->GetLightType();
        vec3 diffuse = light->GetDiffuse();
        vec3 specular = light->GetSpecular();
        vec3 ambient = light->GetAmbient();
        float intensity = light->GetIntensity();

        if (ImGui::Combo("Type", (int*)&lightType, "Directional\0Point\0")) {
            light->SetLightType(lightType);
        }

        if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
            light->SetIntensity(intensity);
        }

        if (lightType == LightType::POINT) {
            DrawPointLightProperties(light, diffuse, specular, ambient);
        }
    }

    static void DrawPointLightProperties(LightComponent* light, vec3& diffuse, vec3& specular, vec3& ambient) {
        float radius = light->GetRadius();
        float constant = light->GetConstant();
        float linear = light->GetLinear();
        float quadratic = light->GetQuadratic();

        float diffuseFloat[3] = { static_cast<float>(diffuse.x), static_cast<float>(diffuse.y), static_cast<float>(diffuse.z) };
        float specularFloat[3] = { static_cast<float>(specular.x), static_cast<float>(specular.y), static_cast<float>(specular.z) };
        float ambientFloat[3] = { static_cast<float>(ambient.x), static_cast<float>(ambient.y), static_cast<float>(ambient.z) };

        if (ImGui::DragFloat("Range", &radius, 0.1f, 0.0f, 1000.0f)) {
            light->SetRadius(radius);
        }
        if (ImGui::DragFloat("Constant", &constant, 0.1f, 0.0f, 1000.0f)) {
            light->SetConstant(constant);
        }
        if (ImGui::DragFloat("Linear", &linear, 0.1f, 0.0f, 1000.0f)) {
            light->SetLinear(linear);
        }
        if (ImGui::DragFloat("Quadratic", &quadratic, 0.1f, 0.0f, 1000.0f)) {
            light->SetQuadratic(quadratic);
        }

        if (ImGui::ColorEdit3("Ambient", ambientFloat)) {
            light->SetAmbient(vec3(ambientFloat[0], ambientFloat[1], ambientFloat[2]));
        }
        if (ImGui::ColorEdit3("Diffuse", diffuseFloat)) {
            light->SetDiffuse(vec3(diffuseFloat[0], diffuseFloat[1], diffuseFloat[2]));
        }
        if (ImGui::ColorEdit3("Specular", specularFloat)) {
            light->SetSpecular(vec3(specularFloat[0], specularFloat[1], specularFloat[2]));
        }
    }
    #pragma endregion

    #pragma region Sound
    static void DrawSoundComponent(SoundComponent* sound) {
        if (!sound) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Sound")) return;

        DrawAudioFilePath(sound);
        DrawSoundProperties(sound);
        DrawPlaybackControls(sound);
    }

    static void DrawAudioFilePath(SoundComponent* sound) {
        char audioPath[256];
        strcpy_s(audioPath, sound->GetAudioPath().c_str());

        if (ImGui::InputText("Audio File", audioPath, sizeof(audioPath))) {
            sound->LoadAudio(audioPath);
        }

        // Drag and drop handler
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                HandleAudioFileDrop(sound, static_cast<const char*>(payload->Data));
            }
            ImGui::EndDragDropTarget();
        }
    }

    static void HandleAudioFileDrop(SoundComponent* sound, const char* path) {
        std::string extension = std::filesystem::path(path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") {
            sound->LoadAudio(path);
        }
    }

    static void DrawSoundProperties(SoundComponent* sound) {
        bool isMusic = sound->IsMusic();
        if (ImGui::Checkbox("Is Music", &isMusic)) {
            if (!sound->GetAudioPath().empty()) {
                sound->LoadAudio(sound->GetAudioPath(), isMusic);
            }
        }

        bool isSpatial = sound->IsSpatial();
        if (ImGui::Checkbox("3D Sound", &isSpatial)) {
            sound->SetSpatial(isSpatial);
        }

        float volume = sound->GetVolume();
        if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f)) {
            sound->SetVolume(volume);
        }

        bool loop = sound->GetLoop();
        if (ImGui::Checkbox("Loop", &loop)) {
            sound->SetLoop(loop);
        }

        bool autoPlay = sound->GetAutoPlay();
        if (ImGui::Checkbox("Auto Play", &autoPlay)) {
            sound->SetAutoPlay(autoPlay);
        }
    }

    static void DrawPlaybackControls(SoundComponent* sound) {
        ImGui::Separator();

        if (sound->IsPlaying()) {
            if (ImGui::Button("Stop")) {
                sound->Stop();
            }
            ImGui::SameLine();
            if (ImGui::Button("Pause")) {
                sound->Pause();
            }
            ImGui::SameLine();
            if (ImGui::Button("Resume")) {
                sound->Resume();
            }
        }
        else {
            if (ImGui::Button("Play")) {
                sound->Play(sound->GetLoop());
            }
        }

        if (sound->IsSpatial()) {
            ImGui::Text("Position is controlled by Transform component");
        }
    }
    #pragma endregion

    #pragma region AudioListener
    static void DrawAudioListenerComponent(AudioListener* listener, GameObject* gameObject) {
        if (!listener) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Audio Listener")) return;

        Transform_Component* transform = gameObject->GetTransform();
        if (transform) {
            glm::dvec3 position = transform->GetPosition();
            float pos[3] = {
                static_cast<float>(position.x),
                static_cast<float>(position.y),
                static_cast<float>(position.z)
            };
            ImGui::DragFloat3("Position", pos, 0.1f);
        }
    }
    #pragma endregion 

    #pragma region MeshCollider
    static void DrawMeshColliderComponent(MeshColliderComponent* collider) {
        if (!collider) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("MeshCollider")) return;

        glm::vec3 colliderPosition = collider->GetColliderPos();
        float pos[3] = { colliderPosition.x, colliderPosition.y, colliderPosition.z };
        if (ImGui::DragFloat3("Collider Position", pos, 0.1f)) {
            collider->SetColliderPos(glm::vec3(pos[0], pos[1], pos[2]));
        }

        glm::vec3 size = collider->GetSize();
        float sizeArray[3] = { size.x, size.y, size.z };
        if (ImGui::DragFloat3("Collider Size", sizeArray, 0.1f, 0.1f, 100.0f)) {
            collider->SetSize(glm::vec3(sizeArray[0], sizeArray[1], sizeArray[2]));
        }

        bool isTrigger = collider->IsTrigger();
        if (ImGui::Checkbox("Is Trigger", &isTrigger)) {
            collider->SetTrigger(isTrigger);
        }
    }
    #pragma endregion

    #pragma region SkeletalAnimation
    static void DrawSkeletalAnimationComponent(SkeletalAnimationComponent* skeletal) 
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Animation")) return;

		ImGui::Text("Animation: %s", skeletal->GetAnimation()->GetName().c_str());
		ImGui::Text("Time: %.1f / %.1f", skeletal->GetAnimator()->GetCurrentMTime(), skeletal->GetAnimation()->GetDuration());
        
        float playSpeed = skeletal->GetAnimator()->GetPlaySpeed();
        ImGui::DragFloat("Speed", &playSpeed, 0.1f, -10.0f, 10.0f);
        skeletal->GetAnimator()->SetPlaySpeed(playSpeed);
		
        bool isPlaying = skeletal->GetAnimationPlayState();
        ImGui::Checkbox("IsPlaying", &isPlaying);
		skeletal->SetAnimationPlayState(isPlaying);
		
        float time = skeletal->GetAnimator()->GetCurrentMTime();
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::SliderFloat("Timeline", &time, 0, skeletal->GetAnimator()->GetCurrentAnimation()->GetDuration());
        ImGui::PopItemFlag();

		ImGui::Text("Number of animations: %d", skeletal->GetAnimations().size());
		int animationIndex = skeletal->GetAnimationIndex();
        ImGui::InputInt("Animation Index", &animationIndex, 1, 1, ImGuiInputTextFlags_CharsDecimal);
        if (animationIndex < 0) animationIndex = 0;
        if (animationIndex >= skeletal->GetAnimations().size()) animationIndex = skeletal->GetAnimations().size()-1;
        if (animationIndex != skeletal->GetAnimationIndex()) 
        {
			skeletal->SetAnimationIndex(animationIndex);
        }

        if (ImGui::Button("ChangeAnimation")) 
        {
            skeletal->SetAnimation(skeletal->GetAnimations().at(animationIndex).get());
			skeletal->GetAnimator()->PlayAnimation(skeletal->GetAnimation());
        }

		ImGui::DragFloat("Blending Value", &skeletal->blendFactor, 0.01f, 0.0, 1.0f);
		ImGui::Checkbox("Is Blending", &skeletal->isBlending);

    }
    #pragma endregion 

    #pragma region Collider
    static void DrawColliderComponent(ColliderComponent* collider) {
        if (!collider) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Collider")) return;

        DrawColliderTransform(collider);
        DrawColliderProperties(collider);
    }

    static void DrawColliderTransform(ColliderComponent* collider) {
        glm::vec3 colliderPosition = collider->GetColliderPos();
        float pos[3] = { colliderPosition.x, colliderPosition.y, colliderPosition.z };
        if (ImGui::DragFloat3("Collider Position", pos, 0.1f)) {
            collider->SetColliderPos(glm::vec3(pos[0], pos[1], pos[2]));
        }

        glm::quat colliderRotation = collider->GetColliderRotation();
        glm::vec3 eulerRotation = glm::eulerAngles(colliderRotation);
        float rot[3] = {
            glm::degrees(eulerRotation.x),
            glm::degrees(eulerRotation.y),
            glm::degrees(eulerRotation.z)
        };
        if (ImGui::DragFloat3("Collider Rotation", rot, 0.1f)) {
            glm::quat newRotation = glm::quat(glm::radians(glm::vec3(rot[0], rot[1], rot[2])));
            collider->SetColliderRotation(newRotation);
        }

        bool snapToPosition = collider->GetSnapToPosition();
        if (ImGui::Checkbox("Snap Position", &snapToPosition)) {
            collider->SetSnapToPosition(snapToPosition);
        }

        bool resetRotation = false;
        if (ImGui::Checkbox("Reset Rotation", &resetRotation) && resetRotation) {
            collider->SetColliderRotation(glm::quat(glm::radians(glm::vec3(0, 0, 0))));
        }
    }

    static void DrawColliderProperties(ColliderComponent* collider) {
        glm::vec3 size = collider->GetSize();
		glm::vec3 offset = collider->GetOffset();
        float sizeArray[3] = { size.x, size.y, size.z };

        if (ImGui::DragFloat3("Collider Size", sizeArray, 0.1f, 0.1f, 100.0f)) {
            collider->SetSize(glm::vec3(sizeArray[0], sizeArray[1], sizeArray[2]));
        }
        bool isTrigger = collider->IsTrigger();
        if (ImGui::Checkbox("Is Trigger", &isTrigger)) {
            collider->SetTrigger(isTrigger);
        }
		if (ImGui::DragFloat3("Offset", &offset[0], 0.1f, -100.0f, 100.0f)) {
			collider->SetOffset(offset);
		}
    }
    #pragma endregion

    #pragma region Rigidbody
    static void DrawRigidbodyComponent(RigidbodyComponent* rigidbody) {
        if (!rigidbody) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Rigidbody")) return;

        DrawRigidbodyProperties(rigidbody);
        DrawRigidbodyPhysics(rigidbody);
    }

    static void DrawRigidbodyProperties(RigidbodyComponent* rigidbody) {
        float mass = rigidbody->GetMass();
        if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.1f, 10.0f)) {
            rigidbody->SetMass(mass);
        }

        bool isKinematic = rigidbody->IsKinematic();
        if (ImGui::Checkbox("Is Kinematic", &isKinematic)) {
            rigidbody->SetKinematic(isKinematic);
        }

        bool freezeRotation = rigidbody->IsFreezed();
        if (ImGui::Checkbox("Freeze Rotation", &freezeRotation)) {
            rigidbody->SetFreezeRotations(freezeRotation);
        }
    }

    static void DrawRigidbodyPhysics(RigidbodyComponent* rigidbody) {
        float friction = rigidbody->GetFriction();
        if (ImGui::DragFloat("Friction", &friction, 0.1f, 0.0f, 10.0f)) {
            rigidbody->SetFriction(friction);
        }

        float damping[2] = { rigidbody->GetDamping().x, rigidbody->GetDamping().y };
        if (ImGui::DragFloat2("Damping (Linear, Angular)", damping, 0.1f, 0.0f, 10.0f)) {
            rigidbody->SetDamping(damping[0], damping[1]);
        }

        glm::vec3 gravity = rigidbody->GetGravity();
        if (ImGui::DragFloat3("Gravity", &gravity[0], 0.1f)) {
            rigidbody->SetGravity(gravity);
        }
    }
#pragma endregion

    #pragma region Shaders
	static void DrawShaderComponent(ShaderComponent* shader) {
		if (!shader) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Shader")) return;

		ShaderType currentType = shader->GetShaderType();
		int shaderType = static_cast<int>(currentType);

		const char* shaderTypes[] = { "Unlit", "PBR" };
		if (ImGui::Combo("Shader Type", &shaderType, shaderTypes, IM_ARRAYSIZE(shaderTypes))) {
			shader->SetShaderType(static_cast<ShaderType>(shaderType));
		}

		// Add texture loading buttons for PBR workflow
		if (static_cast<ShaderType>(shaderType) == ShaderType::PBR) {
			ImGui::Separator();
			ImGui::Text("PBR Textures");

			// Normal map button
			if (ImGui::Button("Load Normal Map")) {
				ImGui::OpenPopup("LoadNormalMap");
			}
			if (ImGui::BeginPopup("LoadNormalMap")) {
				ImGui::Text("Drag and drop a normal map file here");
				ImGui::EndPopup();
			}

			// Handle drag/drop for normal map
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
					const char* path = static_cast<const char*>(payload->Data);
					shader->SetNormalMap(path);
				}
				ImGui::EndDragDropTarget();
			}

			// Metallic map button
			if (ImGui::Button("Load Metallic Map")) {
				ImGui::OpenPopup("LoadMetallicMap");
			}
			if (ImGui::BeginPopup("LoadMetallicMap")) {
				ImGui::Text("Drag and drop a metallic map file here");
				ImGui::EndPopup();
			}

			// Handle drag/drop for metallic map
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
					const char* path = static_cast<const char*>(payload->Data);
					shader->SetMetallicMap(path);
				}
				ImGui::EndDragDropTarget();
			}

			// Roughness map button
			if (ImGui::Button("Load Roughness Map")) {
				ImGui::OpenPopup("LoadRoughnessMap");
			}
			if (ImGui::BeginPopup("LoadRoughnessMap")) {
				ImGui::Text("Drag and drop a roughness map file here");
				ImGui::EndPopup();
			}

			// Handle drag/drop for roughness map
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
					const char* path = static_cast<const char*>(payload->Data);
					shader->SetRoughnessMap(path);
				}
				ImGui::EndDragDropTarget();
			}

			// AO map button
			if (ImGui::Button("Load AO Map")) {
				ImGui::OpenPopup("LoadAOMap");
			}
			if (ImGui::BeginPopup("LoadAOMap")) {
				ImGui::Text("Drag and drop an ambient occlusion map file here");
				ImGui::EndPopup();
			}

			// Handle drag/drop for AO map
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
					const char* path = static_cast<const char*>(payload->Data);
					shader->SetAOMap(path);
				}
				ImGui::EndDragDropTarget();
			}

			// PBR material properties
			ImGui::Separator();
			ImGui::Text("PBR Properties");

			if (!shader->HasMetallicMap()) {
				float metallic = shader->GetMetallic();
				if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) {
					shader->SetMetallic(metallic);
				}
			}

			if (!shader->HasRoughnessMap()) {
				float roughness = shader->GetRoughness();
				if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) {
					shader->SetRoughness(roughness);
				}
			}

			if (!shader->HasAOMap()) {
				float ao = shader->GetAO();
				if (ImGui::SliderFloat("Ambient Occlusion", &ao, 0.0f, 1.0f)) {
					shader->SetAO(ao);
				}
			}
		}
	}
#pragma endregion

    #pragma region Scripting
    static void DrawScriptComponents(GameObject* gameObject) {
        if (gameObject->scriptComponents.empty()) return;

        for (auto& scriptComponent : gameObject->scriptComponents) {
            if (!scriptComponent || !scriptComponent->monoScript) continue;

            ImGui::PushID(scriptComponent.get());
            DrawSingleScriptComponent(scriptComponent.get());
            ImGui::PopID();
        }
    }

    static void DrawSingleScriptComponent(ScriptComponent* scriptComponent) {
        if (!scriptComponent || !scriptComponent->monoScript) return;

        std::string scriptName = mono_class_get_name(mono_object_get_class(scriptComponent->monoScript));

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader(scriptComponent->GetName().c_str())) {
            return;
        }

        ImGui::Text("Script: %s", scriptName.c_str());
        DrawScriptControls(scriptComponent, scriptName);
        DrawScriptFields(scriptComponent);
    }

    static void DrawScriptControls(ScriptComponent* scriptComponent, const std::string& scriptName) {
        if (ImGui::Button("Open Script")) {
            std::string scriptPath = std::filesystem::absolute("../Script/" + scriptName + ".cs").string();
            LOG(LogType::LOG_INFO, "Absolute script path: %s", scriptPath.c_str());

            if (!std::filesystem::exists(scriptPath)) {
                LOG(LogType::LOG_ERROR, "Script file does not exist at path: %s", scriptPath.c_str());
                return;
            }

            HINSTANCE result = ShellExecuteA(nullptr, "open", scriptPath.c_str(), nullptr, nullptr, SW_SHOW);
            if ((int)result <= 32) {
                LOG(LogType::LOG_ERROR, "Failed to open script file: %s. Error code: %d", scriptPath.c_str(), (int)result);
            }
            else {
                LOG(LogType::LOG_INFO, "Successfully opened script: %s", scriptPath.c_str());
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
    }

    static void DrawScriptFields(ScriptComponent* scriptComponent) {
        if (!scriptComponent || !scriptComponent->monoScript) return;

        MonoClass* scriptClass = mono_object_get_class(scriptComponent->monoScript);
        void* iter = nullptr;
        MonoClassField* field = nullptr;

        while ((field = mono_class_get_fields(scriptClass, &iter))) {
            guint32 flags = mono_field_get_flags(field);
            if (flags & MONO_FIELD_ATTR_STATIC) {
                continue;
            }

            const char* fieldName = mono_field_get_name(field);
            MonoType* fieldType = mono_field_get_type(field);
            int typeCode = mono_type_get_type(fieldType);

            ImGui::PushID(field);
            DrawScriptField(scriptComponent->monoScript, field, fieldName, typeCode);
            ImGui::PopID();
        }
    }

    static void DrawScriptField(MonoObject* monoScript, MonoClassField* field, const char* fieldName, int typeCode) {
        if (mono_field_get_flags(field) & MONO_FIELD_ATTR_STATIC) {
            return;
        }

        switch (typeCode) {
        case MONO_TYPE_STRING:
            DrawStringField(monoScript, field, fieldName);
            break;
        case MONO_TYPE_BOOLEAN:
            DrawBoolField(monoScript, field, fieldName);
            break;
        case MONO_TYPE_I4:
            DrawIntField(monoScript, field, fieldName);
            break;
        case MONO_TYPE_R4:
            DrawFloatField(monoScript, field, fieldName);
            break;
        case MONO_TYPE_R8:
            DrawDoubleField(monoScript, field, fieldName);
            break;
        }
    }

    static void DrawStringField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
        MonoString* monoString = nullptr;
        mono_field_get_value(monoScript, field, &monoString);
        std::string value = monoString ? mono_string_to_utf8(monoString) : "";

        char buffer[128];
        strncpy_s(buffer, value.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText(fieldName, buffer, sizeof(buffer))) {
            MonoString* newMonoString = mono_string_new(mono_domain_get(), buffer);
            mono_field_set_value(monoScript, field, newMonoString);
        }
    }

    static void DrawBoolField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
        bool value = false;
        mono_field_get_value(monoScript, field, &value);
        if (ImGui::Checkbox(fieldName, &value)) {
            mono_field_set_value(monoScript, field, &value);
        }
    }

    static void DrawIntField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
        int value = 0;
        mono_field_get_value(monoScript, field, &value);
        if (ImGui::InputInt(fieldName, &value)) {
            mono_field_set_value(monoScript, field, &value);
        }
    }

    static void DrawFloatField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
        float value = 0.0f;
        mono_field_get_value(monoScript, field, &value);
        if (ImGui::InputFloat(fieldName, &value)) {
            mono_field_set_value(monoScript, field, &value);
        }
    }

    static void DrawDoubleField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
        double value = 0.0;
        mono_field_get_value(monoScript, field, &value);
        if (ImGui::InputDouble(fieldName, &value)) {
            mono_field_set_value(monoScript, field, &value);
        }
    }
    #pragma endregion

	#pragma region ParticleFX
	static void DrawParticleSystemComponent(ParticleFX* system) {
		if (!system) return;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		bool isOpen = ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::PopStyleVar();

		if (!isOpen) return;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
		ImGui::Indent(10.0f);

		DrawParticleControls(system);
		DrawParticleEmissionSettings(system);
		DrawParticleParticleSettings(system);
		DrawParticleShapeSettings(system);
		DrawParticleRenderingSettings(system);
		DrawParticleTextureSettings(system);

		ImGui::Unindent(10.0f);
		ImGui::PopStyleVar();
	}

	static void DrawParticleControls(ParticleFX* system) {
		ImGui::BeginGroup();

		// Create a row of buttons
		float width = ImGui::GetContentRegionAvail().x / 3.0f - 2.0f;

		if (system->IsPlaying()) {
			if (ImGui::Button("Stop", ImVec2(width, 0))) {
				system->Stop();
			}
		}
		else {
			if (ImGui::Button("Play", ImVec2(width, 0))) {
				system->Play();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Pause", ImVec2(width, 0))) {
			system->Pause();
		}

		ImGui::SameLine();
		if (ImGui::Button("Emit Burst", ImVec2(width, 0))) {
			system->EmitBurst(20); // Emit 20 particles as a burst
		}

		bool isOneShot = system->IsOneShot();
		if (ImGui::Checkbox("One Shot", &isOneShot)) {
			system->SetOneShot(isOneShot);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Emit particles once and then stop");
		}

		ImGui::EndGroup();
		ImGui::Separator();
	}

	static void DrawParticleEmissionSettings(ParticleFX* system) {
		if (!ImGui::TreeNodeEx("Emission Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

		ImGui::BeginGroup();

		float emissionRate = system->GetEmissionRate();
		if (ImGui::DragFloat("Emission Rate", &emissionRate, 0.1f, 0.1f, 1000.0f, "%.1f")) {
			system->SetEmissionRate(emissionRate);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Particles per second");
		}

		int maxParticles = system->GetMaxParticles();
		if (ImGui::DragInt("Max Particles DOESNT WORK", &maxParticles, 1, 1, 10000)) {
			//system->SetMaxParticles(maxParticles);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Maximum number of particles allowed");
		}

		ImGui::EndGroup();
		ImGui::TreePop();
	}

	static void DrawParticleParticleSettings(ParticleFX* system) {
		if (!ImGui::TreeNodeEx("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

		ImGui::BeginGroup();

		// Lifetime
		float minLifetime = system->GetMinLifetime();
		float maxLifetime = system->GetMaxLifetime();
		if (ImGui::DragFloatRange2("Lifetime", &minLifetime, &maxLifetime, 0.05f, 0.01f, 20.0f)) {
			system->SetParticleLifetime(minLifetime, maxLifetime);
		}

		// Speed
		float minSpeed = system->GetMinSpeed();
		float maxSpeed = system->GetMaxSpeed();
		if (ImGui::DragFloatRange2("Speed", &minSpeed, &maxSpeed, 0.05f, 0.0f, 50.0f)) {
			system->SetParticleSpeed(minSpeed, maxSpeed);
		}

		// Size
		float startSize = system->GetStartSize();
		float endSize = system->GetEndSize();
		if (ImGui::DragFloatRange2("Size", &startSize, &endSize, 0.05f, 0.01f, 10.0f)) {
			system->SetParticleSize(startSize, endSize);
		}

		// Rotation
		float rotationSpeed = system->GetRotationSpeed();
		if (ImGui::DragFloat("Rotation Speed", &rotationSpeed, 0.1f, 0.0f, 10.0f)) {
			system->SetParticleRotation(rotationSpeed);
		}

		// Gravity
		float gravity = system->GetGravity();
		if (ImGui::DragFloat("Gravity", &gravity, 0.01f, -10.0f, 10.0f)) {
			system->SetGravity(gravity);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Negative values make particles rise");
		}

		// Color
		ImGui::Text("Start Color");
		glm::vec3 startColor = system->GetStartColor();
		float startAlpha = system->GetStartAlpha();
		float startColorArr[4] = { startColor.r, startColor.g, startColor.b, startAlpha };
		if (ImGui::ColorEdit4("##StartColor", startColorArr)) {
			system->SetParticleColor(glm::vec3(startColorArr[0], startColorArr[1], startColorArr[2]), system->GetEndColor());
			system->SetParticleAlpha(startColorArr[3], system->GetEndAlpha());
		}

		ImGui::Text("End Color");
		glm::vec3 endColor = system->GetEndColor();
		float endAlpha = system->GetEndAlpha();
		float endColorArr[4] = { endColor.r, endColor.g, endColor.b, endAlpha };
		if (ImGui::ColorEdit4("##EndColor", endColorArr)) {
			system->SetParticleColor(system->GetStartColor(), glm::vec3(endColorArr[0], endColorArr[1], endColorArr[2]));
			system->SetParticleAlpha(system->GetStartAlpha(), endColorArr[3]);
		}

		ImGui::EndGroup();
		ImGui::TreePop();
	}

	static void DrawParticleShapeSettings(ParticleFX* system) {
		if (!ImGui::TreeNodeEx("Shape Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

		ImGui::BeginGroup();

		// Shape type
		static const char* shapeItems[] = {
			"Point",
			"Sphere",
			"Cone",
			"Box",
			"Circle"
		};

		int currentShape = static_cast<int>(system->GetEmitterShape());
		if (ImGui::Combo("Shape", &currentShape, shapeItems, IM_ARRAYSIZE(shapeItems))) {
			system->SetEmitterShape(static_cast<EmitterShape>(currentShape));
		}

		// Shape parameters based on shape type
		EmitterShape shape = system->GetEmitterShape();

		float param1 = system->GetShapeParam1();
		float param2 = system->GetShapeParam2();
		float param3 = system->GetShapeParam3();

		switch (shape) {
		case EmitterShape::POINT:
			ImGui::Text("Point emitter has no parameters");
			break;

		case EmitterShape::SPHERE:
			if (ImGui::DragFloat("Radius", &param1, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			break;

		case EmitterShape::CONE:
			if (ImGui::DragFloat("Radius", &param1, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			if (ImGui::DragFloat("Height", &param2, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			if (ImGui::DragFloat("Angle (degrees)", &param3, 0.1f, 0.1f, 180.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			break;

		case EmitterShape::BOX:
			if (ImGui::DragFloat("Width", &param1, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			if (ImGui::DragFloat("Height", &param2, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			if (ImGui::DragFloat("Depth", &param3, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			break;

		case EmitterShape::CIRCLE:
			if (ImGui::DragFloat("Radius", &param1, 0.01f, 0.001f, 100.0f)) {
				system->SetShapeParameters(param1, param2, param3);
			}
			break;
		}

		ImGui::EndGroup();
		ImGui::TreePop();
	}

	static void DrawParticleRenderingSettings(ParticleFX* system) {
		if (!ImGui::TreeNodeEx("Rendering Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

		ImGui::BeginGroup();

		// Billboard type
		int billboardType = system->GetBillboardType();
		const char* billboardItems[] = {
			"Screen Aligned - Always faces camera",
			"World Aligned - Maintains up vector",
			"Axis Aligned - Rotates around specified axis"
		};

		if (ImGui::Combo("Billboard Type", &billboardType, billboardItems, IM_ARRAYSIZE(billboardItems))) {
			system->SetBillboardType(billboardType);
		}

		// Particle type
		int particleType = static_cast<int>(system->GetParticleType());
		const char* particleTypeItems[] = {
			"Default",
			"Smoke",
			"Fire",
			"Muzzle Flash"
		};

		if (ImGui::Combo("Particle Type", &particleType, particleTypeItems, IM_ARRAYSIZE(particleTypeItems))) {
			system->SetParticleType(static_cast<ParticleType>(particleType));
		}

		// Quick preset buttons
		ImGui::Text("Quick Presets:");

		float width = (ImGui::GetContentRegionAvail().x - 9.0f) / 4.0f; // 3 spaces between buttons

		if (ImGui::Button("Smoke", ImVec2(width, 0))) {
			system->ConfigureSmoke();
		}
		ImGui::SameLine(0, 3);

		if (ImGui::Button("Fire", ImVec2(width, 0))) {
			system->ConfigureFire();
		}
		ImGui::SameLine(0, 3);

		if (ImGui::Button("Muzzle Flash", ImVec2(width, 0))) {
			system->ConfigureMuzzleFlash();
		}
		ImGui::SameLine(0, 3);

		if (ImGui::Button("Dust", ImVec2(width, 0))) {
			system->ConfigureDust();
		}

		// Softness
		float softness = system->GetSoftness();
		if (ImGui::SliderFloat("Edge Softness", &softness, 0.0f, 1.0f)) {
			system->SetSoftness(softness);
		}

		ImGui::EndGroup();
		ImGui::TreePop();
	}

	static void DrawParticleTextureSettings(ParticleFX* system) {
		if (!ImGui::TreeNodeEx("Texture Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

		ImGui::BeginGroup();

		// Main texture
		ImGui::TextUnformatted("Particle Texture");
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::TextUnformatted("Supported formats: .png, .jpg, .jpeg, .bmp");
			ImGui::TextUnformatted("Drag and drop a texture file here");
			ImGui::EndTooltip();
		}

		std::string texturePath = system->GetTexturePath();
		char texturePathBuffer[256] = { 0 };
		if (!texturePath.empty()) {
			strncpy_s(texturePathBuffer, sizeof(texturePathBuffer), texturePath.c_str(), _TRUNCATE);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
		if (ImGui::InputText("##TexturePath", texturePathBuffer, sizeof(texturePathBuffer))) {
			system->SetTexture(texturePathBuffer);
		}
		ImGui::PopStyleVar();

		if (!texturePath.empty()) {
			ImGui::SameLine();
			if (ImGui::Button("Preview##texture")) {
				// Preview texture (implement similar to MeshRenderer) TODO
			}
		}

		ImGui::Button("Drop Texture Here", ImVec2(ImGui::GetContentRegionAvail().x, 30));
		HandleParticleTextureDrop(system, false);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Color gradient texture
		ImGui::TextUnformatted("Color Gradient Texture");
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::TextUnformatted("Horizontal gradient texture for color variation over lifetime");
			ImGui::TextUnformatted("Left side is start of lifetime, right side is end");
			ImGui::EndTooltip();
		}

		std::string gradientPath = system->GetGradientPath();
		char gradientPathBuffer[256] = { 0 };
		if (!gradientPath.empty()) {
			strncpy_s(gradientPathBuffer, sizeof(gradientPathBuffer), gradientPath.c_str(), _TRUNCATE);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
		if (ImGui::InputText("##GradientPath", gradientPathBuffer, sizeof(gradientPathBuffer))) {
			system->SetColorGradient(gradientPathBuffer);
		}
		ImGui::PopStyleVar();

		if (!gradientPath.empty()) {
			ImGui::SameLine();
			if (ImGui::Button("Preview##gradient")) {
				// Preview gradient texture
			}

			ImGui::SameLine();
			if (ImGui::Button("Clear##gradient")) {
				system->DisableColorGradient();
			}
		}

		ImGui::Button("Drop Gradient Here", ImVec2(ImGui::GetContentRegionAvail().x, 30));
		HandleParticleTextureDrop(system, true);

		ImGui::EndGroup();
		ImGui::TreePop();
	}

	static void HandleParticleTextureDrop(ParticleFX* system, bool isGradient) {
		if (!ImGui::BeginDragDropTarget()) return;

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
			const char* path = static_cast<const char*>(payload->Data);
			std::string extension = std::filesystem::path(path).extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

			const std::array<std::string, 5> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".image" };

			if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
				if (isGradient) {
					system->SetColorGradient(path);
				}
				else {
					system->SetTexture(path);
				}
			}
			else {
				LOG(LogType::LOG_WARNING, "Invalid texture format: %s", extension.c_str());
			}
		}

		ImGui::EndDragDropTarget();
	}
	#pragma endregion

    #pragma region Canvas
    static void DrawCanvasComponent(UICanvasComponent* canvas) {
        if (!canvas) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Canvas")) return;

        ImGui::Text("Canvas");
    }
#pragma endregion

    #pragma region Image
    static void DrawImageComponent(UIImageComponent* image) {
        if (!image) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Image")) return;

        ImGui::Text("Image");

        DrawImageFilePath(image);
    }

    static void DrawImageFilePath(UIImageComponent* image) {
        char imagePath[256];
        strcpy_s(imagePath, image->GetImagePath().c_str());

        if (ImGui::InputText("image File", imagePath, sizeof(imagePath))) {
            image->SetTexture(imagePath);
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                HandleImageFileDrop(image, static_cast<const char*>(payload->Data));
            }
            ImGui::EndDragDropTarget();
        }
    }

    static void HandleImageFileDrop(UIImageComponent* image, const char* path) {
        std::string extension = std::filesystem::path(path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == ".jpg" || extension == ".png" || extension == ".img") {
            image->SetTexture(path);
        }
    }

#pragma endregion

    #pragma region RectTransform
    static void DrawRectTransformComponent(UITransformComponent* transform) {
        if (!transform) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("RectTransform")) return;

        ImGui::Text("RectTransform");

        glm::dvec3 currentPosition = transform->GetPosition();
        glm::dvec3 currentRotation = glm::radians(transform->GetRotation());
        glm::dvec3 currentScale = transform->GetScale();

        float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
        float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
        float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

        if (ImGui::DragFloat3("Position", pos, 0.001f, -1.0f, 1.0f)) {
            glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
            glm::dvec3 deltaPos = newPosition - currentPosition;
            transform->Translate(deltaPos);
        }

        if (ImGui::DragFloat3("Rotation", rot, -1.0f)) {
            glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
            glm::dvec3 deltaRot = newRotation - currentRotation;
            transform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
            transform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
            transform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
        }

        if (ImGui::DragFloat3("Scale", sca, 0.001f, -1.0f, 1.0f)) {
            glm::dvec3 newScale = { sca[0], sca[1], sca[2] };
            glm::dvec3 deltaScale = newScale / currentScale;
            transform->Scale(deltaScale);
        }

    }
    
  
#pragma endregion
public:
    static void DrawComponents(GameObject* gameObject, bool& snap, float& snapValue) {
		if (!gameObject) return;

		Transform_Component* transform = gameObject->GetTransform();
		if (transform) {
			DrawTransformComponent(transform, snap, snapValue);
		}

		if (gameObject->HasComponent<MeshRenderer>()) {
			MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
			DrawMeshRendererComponent(meshRenderer);
		}

		if (gameObject->HasComponent<SkeletalAnimationComponent>()) {
			SkeletalAnimationComponent* animationComponent = gameObject->GetComponent<SkeletalAnimationComponent>();
			DrawSkeletalAnimationComponent(animationComponent);
		}

        if (gameObject->HasComponent<CameraComponent>()) {
			CameraComponent* camera = gameObject->GetComponent<CameraComponent>();
			DrawCameraComponent(camera);
		}

        if (gameObject->HasComponent<LightComponent>()) {
			LightComponent* light = gameObject->GetComponent<LightComponent>();
			DrawLightComponent(light);
		}

        if (gameObject->HasComponent<SoundComponent>()) {
			SoundComponent* sound = gameObject->GetComponent<SoundComponent>();
			DrawSoundComponent(sound);
		}

		if (gameObject->HasComponent<AudioListener>()) {
			AudioListener* listener = gameObject->GetComponent<AudioListener>();
			DrawAudioListenerComponent(listener, gameObject);
		}

		if (gameObject->HasComponent<ColliderComponent>()) {
			ColliderComponent* collider = gameObject->GetComponent<ColliderComponent>();
			DrawColliderComponent(collider);
		}
        
        if (gameObject->HasComponent<MeshColliderComponent>()) {
            MeshColliderComponent* meshCollider = gameObject->GetComponent<MeshColliderComponent>();
			DrawMeshColliderComponent(meshCollider);
		}
        
        if (gameObject->HasComponent<RigidbodyComponent>()) {
            RigidbodyComponent* rigidbody = gameObject->GetComponent<RigidbodyComponent>();
			DrawRigidbodyComponent(rigidbody);
		}

		if (gameObject->HasComponent<ParticleFX>()) {
            ParticleFX* emitter = gameObject->GetComponent<ParticleFX>();
			DrawParticleSystemComponent(emitter);
		}

        if (gameObject->HasComponent<UITransformComponent>()) {
            UITransformComponent* uiTransformComponent = gameObject->GetComponent<UITransformComponent>();
            DrawRectTransformComponent(uiTransformComponent);
        }
      
		if (gameObject->HasComponent<ShaderComponent>()) {
			ShaderComponent* shader = gameObject->GetComponent<ShaderComponent>();
			DrawShaderComponent(shader);
		}

		if (gameObject->HasComponent<UICanvasComponent>()) {
            UICanvasComponent* uiCanvasComponent = gameObject->GetComponent<UICanvasComponent>();
			DrawCanvasComponent(uiCanvasComponent);
		}
        if (gameObject->HasComponent<UIImageComponent>()) {
            UIImageComponent* uiImageComponent = gameObject->GetComponent<UIImageComponent>();
            DrawImageComponent(uiImageComponent);
        }
        if (gameObject->scriptComponents.size() > 0) {
			DrawScriptComponents(gameObject);
		}

		//Aqui mas componentes
	}

    static void DrawAddComponentButton(GameObject* gameObject) {
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentMenu");
        }

        if (ImGui::BeginPopup("AddComponentMenu")) {
            DrawAddComponentMenu(gameObject);
            ImGui::EndPopup();
        }
    }

private:
    static void DrawAddComponentMenu(GameObject* gameObject) {
		if (!gameObject) return;

		if (!gameObject->HasComponent<CameraComponent>()) {
			if (ImGui::MenuItem("Camera")) {
				gameObject->AddComponent<CameraComponent>();
			}
		}

		if (!gameObject->HasComponent<MeshRenderer>()) {
			if (ImGui::MenuItem("MeshRenderer")) {
				Application->root->AddMeshRenderer(*gameObject, Mesh::CreateCube(), "Assets/default.png");
			}
		}

		if (!gameObject->HasComponent<LightComponent>()) {
			if (ImGui::MenuItem("Light")) {
				gameObject->AddComponent<LightComponent>();
			}
		}

		if (!gameObject->HasComponent<SoundComponent>()) {
			if (ImGui::MenuItem("Sound")) {
				gameObject->AddComponent<SoundComponent>();
			}
		}

		if (!gameObject->HasComponent<AudioListener>()) {
			if (ImGui::MenuItem("Audio Listener")) {
				gameObject->AddComponent<AudioListener>();
			}
		}

		if (!gameObject->HasComponent<ColliderComponent>()) {
			if (ImGui::MenuItem("Collider")) {
				gameObject->AddComponent<ColliderComponent>(Application->physicsModule);
				gameObject->GetComponent<ColliderComponent>()->Start();
			}
		}
        
        if (!gameObject->HasComponent<MeshColliderComponent>()) {
			if (ImGui::MenuItem("MeshCollider")) {
				gameObject->AddComponent<MeshColliderComponent>(Application->physicsModule);
				gameObject->GetComponent<MeshColliderComponent>()->Start();
			}
		}
        
        if (!gameObject->HasComponent<RigidbodyComponent>()) {
			if (ImGui::MenuItem("RigidBody")) {
				gameObject->AddComponent<RigidbodyComponent>(Application->physicsModule);
				gameObject->GetComponent<RigidbodyComponent>()->Start();
			}
		}

		if (!gameObject->HasComponent<ParticleFX>()) {
			if (ImGui::MenuItem("ParticleEmitter")) {
				gameObject->AddComponent<ParticleFX>();
			}
		}
        
		if (!gameObject->HasComponent<UICanvasComponent>()) {
			if (ImGui::MenuItem("Canvas")) {
                if (!gameObject->HasComponent<UITransformComponent>()) {
                    gameObject->AddComponent<UITransformComponent>();
                }
				gameObject->AddComponent<UICanvasComponent>();
			}
		}
		if (!gameObject->HasComponent<UIImageComponent>()) {
			if (ImGui::MenuItem("Image")) {
				if (!gameObject->HasComponent<UITransformComponent>()) {
					gameObject->AddComponent<UITransformComponent>();
				}				
                gameObject->AddComponent<UIImageComponent>();
				gameObject->GetComponent<UIImageComponent>()->SetTexture("Assets/default.png");
			}
		}
        
		if (!gameObject->HasComponent<UITransformComponent>()) {
			if (ImGui::MenuItem("RectTransform")) {
				gameObject->AddComponent<UITransformComponent>();
			}
		}

		//Aqui mas componentes
	}

    static ImVec2 CalculatePreviewSize(int width, int height) {
        const float maxPreviewSize = 200.0f;
        float aspectRatio = static_cast<float>(width) / height;

        if (width > maxPreviewSize || height > maxPreviewSize) {
            if (aspectRatio > 1.0f) {
                return {maxPreviewSize, maxPreviewSize / aspectRatio};
            }
            return {maxPreviewSize * aspectRatio, maxPreviewSize};
        }

        return {static_cast<float>(width), static_cast<float>(height)};
    }
};

UIInspector::UIInspector(UIType type, std::string name)
    : UIElement(type, name), matrixDirty(false), position(0), rotationRadians(0), scale(1) {
}

UIInspector::~UIInspector() {
}

bool UIInspector::Draw() {
    ImGuiWindowFlags inspectorFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoCollapse;
    ImGuiWindowClass windowClass;
    windowClass.DockingAllowUnclassed = false;

    if (firstDraw) {
        ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
        firstDraw = false;
    }

    ImGui::SetNextWindowClass(&windowClass);

    if (!ImGui::Begin("Inspector", nullptr, inspectorFlags)) {
        ImGui::End();
        return false;
    }

    auto selectedObjects = Application->input->GetSelectedGameObjects();
    if (selectedObjects.empty()) {
        ImGui::Text("No GameObject selected");
        ImGui::End();
        return true;
    }

    GameObject* selectedObject = selectedObjects.back();
    if (!selectedObject) {
        ImGui::Text("Error: Invalid GameObject selected");
        ImGui::End();
        return false;
    }

    DrawGameObjectHeader(selectedObject);
    ImGui::Separator();

    ComponentDrawer::DrawComponents(selectedObject, snap, snapValue);
    ImGui::Separator();

    ComponentDrawer::DrawAddComponentButton(selectedObject);

    ImGui::End();
    return true;
}

void UIInspector::DrawGameObjectHeader(GameObject* gameObject) {
    char newName[128] = {};
    strncpy_s(newName, gameObject->GetName().c_str(), sizeof(newName));

    ImGui::Text("GameObject:");
    if (ImGui::InputText("##GameObjectName", newName, sizeof(newName), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (strlen(newName) > 0) {
            gameObject->SetName(newName);
        }
    }

    ImGui::SameLine();
    ImGui::Checkbox("Static", &gameObject->isStatic);

    ImGui::Separator();

    bool isActive = gameObject->IsActive();
    if (ImGui::Checkbox("Active", &isActive)) {
		gameObject->SetActive(isActive);
	}
}