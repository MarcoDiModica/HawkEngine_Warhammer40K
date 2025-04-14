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
#include "../MyPhysicsEngine/BoxColliderComponent.h"
#include "../MyPhysicsEngine/RigidBodyComponent.h"
#include "../MyPhysicsEngine/MeshColliderComponent.h"
#include "../MyPhysicsEngine/CapsuleColliderComponent.h"
#include "../MyScriptingEngine/ScriptComponent.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "../MyAnimationEngine/SkeletalAnimationComponent.h"

#include <Windows.h>
#include "../MyParticlesEngine/ParticleFX.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "../MyUIEngine/UIImageComponent.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "mono/metadata/debug-helpers.h"
#include "../MyUIEngine/UIButtonComponent.h"

#include <MyGameEngine/ImGuiCurveEditor.h>
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

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;
		const ImVec4 posColor(0.7f, 0.9f, 0.7f, 1.0f);
		const ImVec4 rotColor(0.9f, 0.7f, 0.7f, 1.0f);
		const ImVec4 scaleColor(0.7f, 0.7f, 0.9f, 1.0f);

		ImGui::BeginGroup();

		GameObject* parent = transform->GetOwner()->GetParent();
		bool hasParent = parent != nullptr;
		AlignedProperty("Parent", hasParent ? parent->GetName().c_str() : "None", labelWidth);

		ImGui::Separator();

		glm::dvec3 currentPosition = transform->GetPosition();
		glm::dvec3 currentRotation = glm::radians(transform->GetEulerAngles());
		glm::dvec3 currentScale = transform->GetScale();

		float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
		float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
		float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position");
		ImGui::PopStyleColor();

		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-20);
		bool posChanged = ImGui::DragFloat3("##Position", pos, 0.1f);

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("World position (X, Y, Z)");
			ImGui::EndTooltip();
		}

		ImGui::SameLine();
		if (ImGui::Button("R##PosReset", ImVec2(25, 25))) {
			transform->SetPosition({ 0, 0, 0 });
		}
		else if (posChanged) {
			transform->SetPosition({ pos[0], pos[1], pos[2] });
		}
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation");
		ImGui::PopStyleColor();

		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-20);
		bool rotChanged = ImGui::DragFloat3("##Rotation", rot, 0.1f);

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Rotation in degrees (X, Y, Z)");
			ImGui::EndTooltip();
		}

		ImGui::SameLine();
		if (ImGui::Button("R##RotReset", ImVec2(25, 25))) {
			transform->SetRotation({ 0, 0, 0 });
		}
		else if (rotChanged) {
			transform->SetRotation(glm::radians(glm::dvec3(rot[0], rot[1], rot[2])));
		}
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale");
		ImGui::PopStyleColor();

		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-20);
		bool scaChanged = ImGui::DragFloat3("##Scale", sca, 0.01f, 0.01f, 100.0f);

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Scale multiplier (X, Y, Z)");
			ImGui::EndTooltip();
		}

		ImGui::SameLine();
		if (ImGui::Button("R", ImVec2(25, 25))) {
			transform->SetScale({ 1, 1, 1 });
		}
		else if (scaChanged) {
			transform->SetScale({ sca[0], sca[1], sca[2] });
		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::BeginGroup();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Snap");
		ImGui::SameLine(labelWidth);
		ImGui::Checkbox("##Snap", &snap);

		if (snap) {
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Snap Value");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			ImGui::DragFloat("##SnapValue", &snapValue, 0.1f, 0.1f, 10.0f);
			ImGui::PopItemWidth();
		}

		ImGui::EndGroup();
	}
    #pragma endregion

	#pragma region MeshRenderer
	static void DrawMeshRendererComponent(MeshRenderer* meshRenderer) {
		if (!meshRenderer) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("MeshRenderer")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				meshRenderer->GetOwner()->RemoveComponent<MeshRenderer>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;
		const float previewSize = 24.0f;

		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
			std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh();
			if (mesh) {
				AlignedProperty("Vertices", static_cast<int>(mesh->getModel()->GetModelData().vertexData.size()), labelWidth);
				AlignedProperty("Indices", static_cast<int>(mesh->getModel()->GetModelData().indexData.size()), labelWidth);
			}
			else {
				ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "No mesh assigned");
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto material = meshRenderer->GetMaterial();
			if (!material) {
				ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "No material assigned");
				ImGui::TreePop();
				return;
			}

			const char* shaderTypes[] = { "UNLIT", "PBR" };
			int currentType = static_cast<int>(material->GetShaderType());
			ImGui::Text("Rendering Mode");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::Combo("##ShaderType", &currentType, shaderTypes, IM_ARRAYSIZE(shaderTypes))) {
				material->SetShaderType(static_cast<ShaderType>(currentType));
			}
			ImGui::PopItemWidth();

			ImGui::Separator();
			ImGui::Text("Main Maps");
			ImGui::Spacing();

			// Albedo 
			auto albedoImage = material->imagePtr;
			TexturePreviewSquare(albedoImage, previewSize, [meshRenderer](const char* path) {
				auto newImage = std::make_shared<Image>();
				if (newImage->LoadTexture(path)) {
					meshRenderer->SetImage(newImage);
				}
				});
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Albedo");

			vec4 matColor = material->GetColor();
			float colorArray[4] = {
				static_cast<float>(matColor.x),
				static_cast<float>(matColor.y),
				static_cast<float>(matColor.z),
				static_cast<float>(matColor.w)
			};

			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::ColorEdit4("##AlbedoColor", colorArray)) {
				vec4 newColor(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
				material->SetColor(newColor);
			}
			ImGui::PopItemWidth();

			ImGui::Spacing();

			if (material->GetShaderType() == ShaderType::PBR) {
				// Metallic 
				auto metallicImage = material->metallicMapPtr;
				TexturePreviewSquare(metallicImage, previewSize, [meshRenderer](const char* path) {
					auto newImage = std::make_shared<Image>();
					if (newImage->LoadTexture(path)) {
						meshRenderer->GetMaterial()->metallicMapPtr = newImage;
					}
					});
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Metallic");

				float metallic = material->metallic;
				ImGui::SameLine(labelWidth);
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat("##Metallic", &metallic, 0.0f, 1.0f)) {
					material->metallic = metallic;
				}
				ImGui::PopItemWidth();

				ImGui::Spacing();

				// Roughness
				auto roughnessImage = material->roughnessMapPtr;
				TexturePreviewSquare(roughnessImage, previewSize, [meshRenderer](const char* path) {
					auto newImage = std::make_shared<Image>();
					if (newImage->LoadTexture(path)) {
						meshRenderer->GetMaterial()->roughnessMapPtr = newImage;
					}
					});
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Roughness");

				float smoothness = 1.0f - material->roughness;
				ImGui::SameLine(labelWidth);
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat("##Roughness", &smoothness, 0.0f, 1.0f)) {
					material->roughness = 1.0f - smoothness;
				}
				ImGui::PopItemWidth();

				ImGui::Spacing();

				// Normal Map
				auto normalMapImage = material->normalMapPtr;
				TexturePreviewSquare(normalMapImage, previewSize, [meshRenderer](const char* path) {
					auto newImage = std::make_shared<Image>();
					if (newImage->LoadTexture(path)) {
						meshRenderer->GetMaterial()->normalMapPtr = newImage;
					}
					});
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Normal Map");

				float normalMapIntensity = 1.0f;
				ImGui::SameLine(labelWidth);
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat("##NormalMapIntensity", &normalMapIntensity, 0.0f, 2.0f);
				ImGui::PopItemWidth();

				ImGui::Spacing();

				//// Height Map (placeholder)
				//TexturePreviewSquare(nullptr, previewSize, [meshRenderer](const char* path) {
				//	// Implementation for when height map functionality is added
				//	});
				//ImGui::SameLine();
				//ImGui::AlignTextToFramePadding();
				//ImGui::Text("Height Map");

				//ImGui::Spacing();

				// Occlusion
				auto aoImage = material->aoMapPtr;
				TexturePreviewSquare(aoImage, previewSize, [meshRenderer](const char* path) {
					auto newImage = std::make_shared<Image>();
					if (newImage->LoadTexture(path)) {
						meshRenderer->GetMaterial()->aoMapPtr = newImage;
					}
					});
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Occlusion");

				float ao = material->ao;
				ImGui::SameLine(labelWidth);
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat("##AO", &ao, 0.0f, 1.0f)) {
					material->ao = ao;
				}
				ImGui::PopItemWidth();

				ImGui::Spacing();

				//// Emission (placeholder)
				//TexturePreviewSquare(nullptr, previewSize, [meshRenderer](const char* path) {
				//	// Implementation for when emission map functionality is added
				//	});
				//ImGui::SameLine();
				//ImGui::AlignTextToFramePadding();
				//ImGui::Text("Emission");

				//float emission = 0.0f;
				//ImGui::SameLine(labelWidth);
				//ImGui::PushItemWidth(-1);
				//ImGui::SliderFloat("##Emission", &emission, 0.0f, 1.0f);
				//ImGui::PopItemWidth();

				//ImGui::Spacing();

				//TexturePreviewSquare(nullptr, previewSize, [meshRenderer](const char* path) {
				//	// Implementation for when detail mask functionality is added
				//	});
				//ImGui::SameLine();
				//ImGui::AlignTextToFramePadding();
				//ImGui::Text("Detail Mask");

				//ImGui::Spacing();

				float tonemapStrength = material->GetTonemapStrength();
				ImGui::Text("Tonemap Strength");
				ImGui::SameLine(labelWidth);
				ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat("##Tonemap", &tonemapStrength, 0.0f, 10.0f)) {
					material->SetTonemapStrength(tonemapStrength);
				}
				ImGui::PopItemWidth();
			}

			ImGui::TreePop();
		}
	}

	template<typename Callback>
	static void TexturePreviewSquare(std::shared_ptr<Image> image, float size, Callback onTextureLoaded) {
		ImGui::PushID((void*)image.get());

		if (image && image->id() != 0) {
			ImGui::Image((void*)(intptr_t)image->id(), ImVec2(size, size));

			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				float previewScale = 8.0f; 
				ImVec2 largePreviewSize(size * previewScale, size * previewScale);
				ImVec2 constrainedSize = CalculatePreviewSize(image->width(), image->height(), 300);
				ImGui::Image((void*)(intptr_t)image->id(), constrainedSize);
				ImGui::Text("%dx%d", image->width(), image->height());
				ImGui::EndTooltip();
			}

			if (ImGui::BeginPopupContextItem("TextureContextMenu")) {
				ImGui::Text("Texture Options");
				ImGui::Separator();
				if (ImGui::MenuItem("Clear")) {
					//TODO
				}
				ImGui::EndPopup();
			}
		}
		else {
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRect(p, ImVec2(p.x + size, p.y + size), IM_COL32(180, 180, 180, 255));

			ImGui::Button("##empty", ImVec2(size, size));

			if (ImGui::BeginPopupContextItem("EmptyTextureContextMenu")) {
				ImGui::EndPopup();
			}
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
				const char* path = static_cast<const char*>(payload->Data);
				std::string extension = std::filesystem::path(path).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

				const std::array<std::string, 5> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".tga" };
				if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
					onTextureLoaded(path);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopID();
	}

	static ImVec2 CalculatePreviewSize(int width, int height, float maxSize = 300.0f) {
		float aspect = (float)width / (float)height;
		ImVec2 result;

		if (width >= height) {
			result.x = std::min((float)width, maxSize);
			result.y = result.x / aspect;
		}
		else {
			result.y = std::min((float)height, maxSize);
			result.x = result.y * aspect;
		}

		return result;
	}

	static void AlignedProperty(const char* label, const char* value, float labelWidth) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
		ImGui::SameLine(labelWidth);
		ImGui::Text("%s", value);
	}

	static void AlignedProperty(const char* label, int value, float labelWidth) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
		ImGui::SameLine(labelWidth);
		ImGui::Text("%d", value);
	}

	static void AlignedProperty(const char* label, float value, float labelWidth) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
		ImGui::SameLine(labelWidth);
		ImGui::Text("%.2f", value);
	}

	static bool AlignedSliderProperty(const char* label, float& value, float min, float max, float labelWidth) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		bool changed = ImGui::SliderFloat(("##" + std::string(label)).c_str(), &value, min, max);
		ImGui::PopItemWidth();
		return changed;
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

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		bool orthographic = camera->IsOrthographic();
		float orthoSize = camera->GetOrthoSize();
		float fov = camera->GetFOV();
		auto nearPlane = static_cast<float>(camera->GetNearPlane());
		auto farPlane = static_cast<float>(camera->GetFarPlane());
		bool frustum = camera->frustrumCullingEnabled;
		bool frustumRepresentation = camera->frustrumRepresentation;
		int priority = camera->GetPriority();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Projection");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("Orthographic", &orthographic)) {
			camera->orthographic = orthographic;
		}

		if (orthographic) {
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Size");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Size", &orthoSize, 0.1f, 0.1f, 100.0f)) {
				camera->SetOrthoSize(orthoSize);
			}
			ImGui::PopItemWidth();
		}
		else {
			float fovDeg = glm::degrees(camera->GetFOV());
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Field of View");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::SliderFloat("##FOV", &fovDeg, 1.0f, 179.0f, "%.1f deg")) {
				camera->SetFOV(glm::radians(fovDeg));
			}
			ImGui::PopItemWidth();
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Near Plane");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##NearPlane", &nearPlane, 0.1f, 0.1f, farPlane - 0.1f)) {
			camera->SetNearPlane(nearPlane);
		}
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Far Plane");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##FarPlane", &farPlane, 0.1f, nearPlane + 0.1f, 1000.0f)) {
			camera->SetFarPlane(farPlane);
		}
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Priority");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragInt("##Priority", &priority, 0.1f, 1, 10)) {
			camera->SetPriority(priority);
			Application->root->UpdateCameraPriority();
		}
		ImGui::PopItemWidth();

		ImGui::Separator();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Frustum Culling");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##FrustumCulling", &frustum)) {
			camera->frustrumCullingEnabled = frustum;
		}

		if (frustum) {
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Show Frustum");
			ImGui::SameLine(labelWidth);
			if (ImGui::Checkbox("##ShowFrustum", &frustumRepresentation)) {
				camera->frustrumRepresentation = frustumRepresentation;
			}
		}

		ImGui::Separator();

		GameObject* followTarget = camera->followTarget;
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Follow Target");
		ImGui::SameLine(labelWidth);
		ImGui::Text("%s", followTarget ? followTarget->GetName().c_str() : "None");

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
				GameObject* target = static_cast<GameObject*>(payload->Data);
				camera->followTarget = target;
			}
			ImGui::EndDragDropTarget();
		}

		if (followTarget) {
			float followDistance = static_cast<float>(camera->GetDistance());
			glm::dvec3 followOffset = camera->GetOffset();
			float followSmoothness = static_cast<float>(camera->followSmoothness);
			bool followX = camera->followX;
			bool followY = camera->followY;
			bool followZ = camera->followZ;

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Distance");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Distance", &followDistance, 0.1f, 0.1f, 100.0f)) {
				camera->SetDistance(followDistance);
			}
			ImGui::PopItemWidth();

			float offset[3] = { followOffset.x, followOffset.y, followOffset.z };
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Offset");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat3("##Offset", offset, 0.1f)) {
				camera->SetOffset(glm::dvec3(offset[0], offset[1], offset[2]));
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Smoothness");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Smoothness", &followSmoothness, 0.01f, 0.01f, 1.0f)) {
				camera->followSmoothness = followSmoothness;
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Follow Axes");
			ImGui::SameLine(labelWidth);

			float axisWidth = (windowWidth - labelWidth - 8) / 3.0f;

			if (ImGui::Checkbox("X##FollowX", &followX)) {
				camera->followX = followX;
			}
			ImGui::SameLine(0, 4);

			if (ImGui::Checkbox("Y##FollowY", &followY)) {
				camera->followY = followY;
			}
			ImGui::SameLine(0, 4);

			if (ImGui::Checkbox("Z##FollowZ", &followZ)) {
				camera->followZ = followZ;
			}

			ImGui::SetCursorPosX(labelWidth);
			if (ImGui::Button("Clear Follow Target", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				camera->followTarget = nullptr;
			}
		}

		ImGui::Separator();

		ImGui::Text("Camera Shake");

		float shakeIntensity = camera->shakeIntensity;
		float shakeDuration = camera->shakeDuration;
		float shakeFrequency = camera->shakeFrequency;

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Intensity");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##ShakeIntensity", &shakeIntensity, 0.1f, 0.0f, 10.0f)) {
			camera->shakeIntensity = shakeIntensity;
		}
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Duration");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##ShakeDuration", &shakeDuration, 0.1f, 0.0f, 10.0f)) {
			camera->shakeDuration = shakeDuration;
		}
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Frequency");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##ShakeFrequency", &shakeFrequency, 0.1f, 0.0f, 10.0f)) {
			camera->shakeFrequency = shakeFrequency;
		}
		ImGui::PopItemWidth();

		ImGui::SetCursorPosX(labelWidth);
		if (ImGui::Button("Test Shake", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			camera->Shake(shakeIntensity, shakeDuration, shakeFrequency);
		}
	}
    #pragma endregion

    #pragma region Light
	static void DrawLightComponent(LightComponent* light) {
		if (!light) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Light")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				light->GetOwner()->RemoveComponent<LightComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		ImGui::BeginGroup();

		LightType lightType = light->GetLightType();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Light Type");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::Combo("##Type", (int*)&lightType, "Directional\0Point\0")) {
			light->SetLightType(lightType);
		}
		ImGui::PopItemWidth();

		float intensity = light->GetIntensity();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Intensity");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Intensity", &intensity, 0.1f, 0.0f, 100.0f)) {
			light->SetIntensity(intensity);
		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();

		if (lightType == LightType::POINT) {
			ImGui::Separator();
			ImGui::Text("Point Light Properties");
			ImGui::Spacing();

			ImGui::BeginGroup();

			vec3 diffuse = light->GetDiffuse();
			vec3 specular = light->GetSpecular();
			vec3 ambient = light->GetAmbient();
			float radius = light->GetRadius();
			float constant = light->GetConstant();
			float linear = light->GetLinear();
			float quadratic = light->GetQuadratic();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Range");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Range", &radius, 0.1f, 0.0f, 1000.0f)) {
				light->SetRadius(radius);
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Constant");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Constant", &constant, 0.01f, 0.0f, 10.0f)) {
				light->SetConstant(constant);
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Linear");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Linear", &linear, 0.01f, 0.0f, 10.0f)) {
				light->SetLinear(linear);
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Quadratic");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Quadratic", &quadratic, 0.01f, 0.0f, 10.0f)) {
				light->SetQuadratic(quadratic);
			}
			ImGui::PopItemWidth();

			ImGui::Separator();
			ImGui::Text("Colors");
			ImGui::Spacing();

			float ambientFloat[3] = { static_cast<float>(ambient.x), static_cast<float>(ambient.y), static_cast<float>(ambient.z) };
			float diffuseFloat[3] = { static_cast<float>(diffuse.x), static_cast<float>(diffuse.y), static_cast<float>(diffuse.z) };
			float specularFloat[3] = { static_cast<float>(specular.x), static_cast<float>(specular.y), static_cast<float>(specular.z) };

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Ambient");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::ColorEdit3("##Ambient", ambientFloat)) {
				light->SetAmbient(vec3(ambientFloat[0], ambientFloat[1], ambientFloat[2]));
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Diffuse");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::ColorEdit3("##Diffuse", diffuseFloat)) {
				light->SetDiffuse(vec3(diffuseFloat[0], diffuseFloat[1], diffuseFloat[2]));
			}
			ImGui::PopItemWidth();

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Specular");
			ImGui::SameLine(labelWidth);
			ImGui::PushItemWidth(-1);
			if (ImGui::ColorEdit3("##Specular", specularFloat)) {
				light->SetSpecular(vec3(specularFloat[0], specularFloat[1], specularFloat[2]));
			}
			ImGui::PopItemWidth();

			ImGui::EndGroup();
		}
	}
    #pragma endregion

    #pragma region Sound
    static void DrawSoundComponent(SoundComponent* sound) {
        if (!sound) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Sound")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				sound->GetOwner()->RemoveComponent<SoundComponent>();
			}
			ImGui::EndPopup();
		}

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

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				listener->GetOwner()->RemoveComponent<AudioListener>();
			}
			ImGui::EndPopup();
		}

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

    #pragma region SkeletalAnimation
	static void DrawSkeletalAnimationComponent(SkeletalAnimationComponent* skeletal)
	{
		if (!skeletal) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Animation")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				skeletal->GetOwner()->RemoveComponent<SkeletalAnimationComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		ImGui::BeginGroup();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Current Animation");
		ImGui::SameLine(labelWidth);
		ImGui::Text("%s", skeletal->GetAnimation()->GetName().c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Time");
		ImGui::SameLine(labelWidth);
		ImGui::Text("%.1f / %.1f", skeletal->GetAnimator()->GetCurrentMTime(), skeletal->GetAnimation()->GetDuration());

		float playSpeed = skeletal->GetAnimator()->GetPlaySpeed();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Speed");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Speed", &playSpeed, 0.1f, -10.0f, 10.0f)) {
			skeletal->GetAnimator()->SetPlaySpeed(playSpeed);
		}
		ImGui::PopItemWidth();

		bool isPlaying = skeletal->GetAnimationPlayState();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Playing");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##IsPlaying", &isPlaying)) {
			skeletal->SetAnimationPlayState(isPlaying);
		}

		float time = skeletal->GetAnimator()->GetCurrentMTime();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Timeline");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::SliderFloat("##Timeline", &time, 0, skeletal->GetAnimator()->GetCurrentAnimation()->GetDuration());
		ImGui::PopItemFlag();
		ImGui::PopItemWidth();

		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Text("Animation Selection");
		ImGui::Spacing();

		ImGui::BeginGroup();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Total Animations");
		ImGui::SameLine(labelWidth);
		ImGui::Text("%d", skeletal->GetAnimations().size());

		int animationIndex = skeletal->GetAnimationIndex();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Animation Index");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::InputInt("##AnimationIndex", &animationIndex, 1, 1, ImGuiInputTextFlags_CharsDecimal)) {
			if (animationIndex < 0) animationIndex = 0;
			if (animationIndex >= skeletal->GetAnimations().size()) animationIndex = skeletal->GetAnimations().size() - 1;
			if (animationIndex != skeletal->GetAnimationIndex()) {
				skeletal->SetAnimationIndex(animationIndex);
			}
		}
		ImGui::PopItemWidth();

		ImGui::SetCursorPosX(labelWidth);
		if (ImGui::Button("Change Animation", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			skeletal->SetAnimation(skeletal->GetAnimations().at(animationIndex).get());
			skeletal->GetAnimator()->PlayAnimation(skeletal->GetAnimation());
		}

		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Text("Blending Options");
		ImGui::Spacing();

		ImGui::BeginGroup();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Blend Factor");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		ImGui::DragFloat("##BlendFactor", &skeletal->blendFactor, 0.01f, 0.0f, 1.0f);
		ImGui::PopItemWidth();

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Enable Blending");
		ImGui::SameLine(labelWidth);
		ImGui::Checkbox("##IsBlending", &skeletal->isBlending);

		ImGui::EndGroup();
	}
    #pragma endregion 

	#pragma region Physics
	static void DrawColliderComponent(BoxColliderComponent* collider) {
		if (!collider) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Box Collider")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				collider->GetOwner()->RemoveComponent<BoxColliderComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		ImGui::BeginGroup();

		bool isTrigger = collider->IsTrigger();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Trigger");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##IsTrigger", &isTrigger)) {
			collider->SetTrigger(isTrigger);
		}

		glm::vec3 offset = collider->GetOffset();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Offset");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Offset", &offset[0], 0.1f, -100.0f, 100.0f)) {
			collider->SetOffset(offset);
		}
		ImGui::PopItemWidth();

		glm::vec3 size = collider->GetSize();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Size");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Size", &size[0], 0.1f, 0.1f, 100.0f)) {
			collider->SetSize(size);
		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();
	}

	static void DrawMeshColliderComponent(MeshColliderComponent* collider) {
		if (!collider) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Mesh Collider")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				collider->GetOwner()->RemoveComponent<MeshColliderComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		ImGui::BeginGroup();

		bool isTrigger = collider->IsTrigger();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Trigger");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##IsTrigger", &isTrigger)) {
			collider->SetTrigger(isTrigger);
		}

		glm::vec3 offset = collider->GetOffset();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Offset");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Offset", &offset[0], 0.1f)) {
			collider->SetOffset(offset);
		}
		ImGui::PopItemWidth();

		glm::vec3 size = collider->GetSize();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Size");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Size", &size[0], 0.1f, 0.1f, 100.0f)) {
			collider->SetSize(size);
		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();
	}

	static void DrawCapsuleColliderComponent(CapsuleColliderComponent* collider) {
		if (!collider) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Capsule Collider")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				collider->GetOwner()->RemoveComponent<CapsuleColliderComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		ImGui::BeginGroup();

		bool isTrigger = collider->IsTrigger();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Trigger");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##IsTrigger", &isTrigger)) {
			collider->SetTrigger(isTrigger);
		}

		glm::vec3 offset = collider->GetOffset();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Offset");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Offset", &offset[0], 0.1f, -100.0f, 100.0f)) {
			collider->SetOffset(offset);
		}
		ImGui::PopItemWidth();

		glm::vec3 size = collider->GetSize();
		float sizeArray[2] = { size.x, size.y };
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Size (Radius, Height)");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat2("##Size", sizeArray, 0.1f, 0.1f, 100.0f)) {
			collider->SetSize(glm::vec3(sizeArray[0], sizeArray[1], size.z));
		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();
	}

	static void DrawRigidbodyComponent(RigidbodyComponent* rigidbody) {
		if (!rigidbody) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Rigidbody")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				rigidbody->GetOwner()->RemoveComponent<RigidbodyComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

		ImGui::BeginGroup();

		float mass = rigidbody->GetMass();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Mass");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Mass", &mass, 0.1f, 0.1f, 10000.0f)) {
			rigidbody->SetMass(mass);
		}
		ImGui::PopItemWidth();

		float friction = rigidbody->GetFriction();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Friction");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Friction", &friction, 0.01f, 0.0f, 10.0f)) {
			rigidbody->SetFriction(friction);
		}
		ImGui::PopItemWidth();

		glm::vec3 gravity = rigidbody->GetGravity();
		float gravityY = gravity.y;
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Gravity");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Gravity", &gravityY, 0.1f)) {
			gravity.y = gravityY;
			rigidbody->SetGravity(gravity);
		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Text("Physics Properties");
		ImGui::Spacing();

		ImGui::BeginGroup();

		bool isKinematic = rigidbody->IsKinematic();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Kinematic");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##IsKinematic", &isKinematic)) {
			rigidbody->SetKinematic(isKinematic);
		}

		bool freezeRotation = rigidbody->IsFreezed();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Freeze Rotation");
		ImGui::SameLine(labelWidth);
		if (ImGui::Checkbox("##FreezeRotation", &freezeRotation)) {
			rigidbody->SetFreezeRotations(freezeRotation);
		}

		float damping[2] = { rigidbody->GetDamping().x, rigidbody->GetDamping().y };
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Damping");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat2("##Damping", damping, 0.01f, 0.0f, 10.0f)) {
			rigidbody->SetDamping(damping[0], damping[1]);
		}
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Linear and Angular Damping");
			ImGui::EndTooltip();
		}

		ImGui::EndGroup();
	}
	#pragma endregion

	#pragma region Scripting
	class MonoFieldHelper {
	public:
		static bool IsPublicField(MonoClassField* field) {
			return (mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC) != 0;
		}

		static bool IsStaticField(MonoClassField* field) {
			return (mono_field_get_flags(field) & MONO_FIELD_ATTR_STATIC) != 0;
		}

		static std::string GetStringValue(MonoObject* obj, MonoClassField* field) {
			MonoString* monoString = nullptr;
			mono_field_get_value(obj, field, &monoString);
			if (!monoString) return "";

			char* cstr = mono_string_to_utf8(monoString);
			std::string result(cstr);
			mono_free(cstr);
			return result;
		}

		static void SetStringValue(MonoObject* obj, MonoClassField* field, const std::string& value) {
			MonoString* monoString = mono_string_new(mono_domain_get(), value.c_str());
			mono_field_set_value(obj, field, monoString);
		}

		static bool OpenScriptFile(const std::string& path) {
			if (!std::filesystem::exists(path)) {
				return false;
			}

#ifdef _WIN32
			HINSTANCE result = ShellExecuteA(nullptr, "open", path.c_str(), nullptr, nullptr, SW_SHOW);
			return ((intptr_t)result > 32);
#elif defined(__APPLE__)
			std::string command = "open \"" + path + "\"";
			return system(command.c_str()) == 0;
#else // Linux and others
			std::string command = "xdg-open \"" + path + "\"";
			return system(command.c_str()) == 0;
#endif
		}
	};

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

		MonoClass* scriptClass = mono_object_get_class(scriptComponent->monoScript);
		if (!scriptClass) {
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: Invalid script class");
			return;
		}

		std::string scriptName = mono_class_get_name(scriptClass);

		std::string headerName = scriptComponent->GetTypeName();
		if (scriptComponent->HasErrors()) {
			headerName += " [ERROR]";
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		bool isOpen = ImGui::CollapsingHeader(headerName.c_str());

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				scriptComponent->GetOwner()->scriptComponents.erase(
					std::remove_if(scriptComponent->GetOwner()->scriptComponents.begin(),
						scriptComponent->GetOwner()->scriptComponents.end(),
						[&](const auto& component) { return component.get() == scriptComponent; }),
					scriptComponent->GetOwner()->scriptComponents.end());
			}
			ImGui::EndPopup();
		}

		if (scriptComponent->HasErrors()) {
			ImGui::PopStyleColor();
		}

		if (!isOpen) return;

		if (scriptComponent->HasErrors()) {
			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			ImGui::TextWrapped("This script has errors. Check the console for details.");
			ImGui::PopStyleColor();
			ImGui::Spacing();
		}

		if (ImGui::Button("Open Script")) {
			std::string scriptPath = std::filesystem::absolute(
				std::filesystem::path(std::string("../Script/") + scriptName + ".cs")).string();

			if (MonoFieldHelper::OpenScriptFile(scriptPath)) {
				LOG(LogType::LOG_INFO, "Opened script: %s", scriptName.c_str());
			}
			else {
				ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Failed to open script file!");
				LOG(LogType::LOG_ERROR, "Failed to open script file: %s", scriptPath.c_str());
			}
		}

		static bool showPublicOnly = true;
		ImGui::SameLine();
		ImGui::Checkbox("Public Fields Only", &showPublicOnly);

		ImGui::Separator();

		DrawScriptFields(scriptComponent, showPublicOnly);
	}

	static void DrawScriptFields(ScriptComponent* scriptComponent) {
		DrawScriptFields(scriptComponent, true);
	}

	static void DrawScriptFields(ScriptComponent* scriptComponent, bool publicOnly = true) {
		if (!scriptComponent || !scriptComponent->monoScript) return;

		MonoClass* scriptClass = mono_object_get_class(scriptComponent->monoScript);
		void* iter = nullptr;
		MonoClassField* field = nullptr;
		bool hasFields = false;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
		ImGui::Indent(10.0f);

		while ((field = mono_class_get_fields(scriptClass, &iter))) {
			if (MonoFieldHelper::IsStaticField(field)) {
				continue;
			}

			if (publicOnly && !MonoFieldHelper::IsPublicField(field)) {
				continue;
			}

			const char* fieldName = mono_field_get_name(field);
			MonoType* fieldType = mono_field_get_type(field);
			int typeCode = mono_type_get_type(fieldType);

			ImGui::PushID(field);
			ImGui::AlignTextToFramePadding();

			switch (typeCode) {
			case MONO_TYPE_BOOLEAN:
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.5f, 1.0f));
				break;
			case MONO_TYPE_I4:
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 1.0f, 1.0f));
				break;
			case MONO_TYPE_R4:
			case MONO_TYPE_R8:
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.7f, 0.4f, 1.0f));
				break;
			case MONO_TYPE_STRING:
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.5f, 1.0f));
				break;
			default:
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
				break;
			}

			float availWidth = ImGui::GetContentRegionAvail().x;
			float labelWidth = std::min(150.0f, availWidth * 0.4f);

			ImGui::Text("%s", fieldName);
			ImGui::SameLine(labelWidth);
			ImGui::SetNextItemWidth(availWidth - labelWidth);

			DrawScriptField(scriptComponent->monoScript, field, fieldName, typeCode);
			ImGui::PopStyleColor();
			ImGui::PopID();

			hasFields = true;
		}

		if (!hasFields) {
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No fields to display");
		}

		ImGui::Unindent(10.0f);
		ImGui::PopStyleVar();
	}

	static void DrawScriptField(MonoObject* monoScript, MonoClassField* field, const char* fieldName, int typeCode) {
		try {
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
			case MONO_TYPE_VALUETYPE:
			{
				MonoType* fieldType = mono_field_get_type(field);
				MonoClass* fieldClass = mono_class_from_mono_type(fieldType);
				const char* className = mono_class_get_name(fieldClass);
				const char* nameSpace = mono_class_get_namespace(fieldClass);

				if (strcmp(nameSpace, "System.Numerics") == 0) {
					if (strcmp(className, "Vector2") == 0) {
						DrawVector2Field(monoScript, field, fieldName);
						break;
					}
					else if (strcmp(className, "Vector3") == 0) {
						DrawVector3Field(monoScript, field, fieldName);
						break;
					}
					else if (strcmp(className, "Vector4") == 0) {
						DrawVector4Field(monoScript, field, fieldName);
						break;
					}
				}

				if (mono_class_is_enum(fieldClass)) {
					DrawEnumField(monoScript, field, fieldName, fieldClass);
					break;
				}

				ImGui::TextDisabled("(Unsupported value type: %s.%s)", nameSpace, className);
			}
			break;
			case MONO_TYPE_CLASS:
			{
				MonoType* fieldType = mono_field_get_type(field);
				MonoClass* fieldClass = mono_class_from_mono_type(fieldType);
				const char* className = mono_class_get_name(fieldClass);
				const char* nameSpace = mono_class_get_namespace(fieldClass);

				if (strcmp(className, "GameObject") == 0 && strcmp(nameSpace, "HawkEngine") == 0) {
					DrawGameObjectField(monoScript, field, fieldName);
				}
				else if (strcmp(className, "Transform") == 0 && strcmp(nameSpace, "HawkEngine") == 0) {
					DrawComponentField(monoScript, field, fieldName, "Transform", "HawkEngine.Transform");
				}
				else if (strcmp(className, "Camera") == 0 && strcmp(nameSpace, "HawkEngine") == 0) {
					DrawComponentField(monoScript, field, fieldName, "Camera", "HawkEngine.Camera");
				}
				else {
					ImGui::TextDisabled("(Unsupported object type: %s.%s)", nameSpace, className);
				}
			}
			break;
			default:
				ImGui::TextDisabled("(Unsupported type)");
				break;
			}
		}
		catch (...) {
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error reading field");
		}
	}

	static void DrawComponentField(MonoObject* monoScript, MonoClassField* field, const char* fieldName, const char* componentName, const char* componentNamespace) {
		MonoObject* fieldValue = nullptr;
		mono_field_get_value(monoScript, field, &fieldValue);

		if (fieldValue == nullptr) {
			ImGui::Text("None");
			return;
		}

		MonoClass* fieldClass = mono_object_get_class(fieldValue);
		const char* className = mono_class_get_name(fieldClass);
		const char* nameSpace = mono_class_get_namespace(fieldClass);

		if (strcmp(className, componentName) == 0 && strcmp(nameSpace, componentNamespace) == 0) {
			ImGui::Text("%s", className);
			return;
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
				MonoObject* newFieldValue = *(MonoObject**)payload->Data;
				if (newFieldValue) {
					mono_field_set_value(monoScript, field, &newFieldValue);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::TextDisabled("(Unsupported type: %s.%s)", nameSpace, className);
	}

	static void DrawEnumField(MonoObject* monoScript, MonoClassField* field, const char* fieldName, MonoClass* enumClass) {
		int enumValue = 0;
		mono_field_get_value(monoScript, field, &enumValue);

		const char* enumTypeName = mono_class_get_name(enumClass);

		MonoType* enumType = mono_class_get_type(enumClass);
		void* iter = NULL;
		MonoClassField* enumField = NULL;
		std::vector<std::string> enumNames;

		while ((enumField = mono_class_get_fields(enumClass, &iter))) {
			if (mono_field_get_flags(enumField) & MONO_FIELD_ATTR_STATIC) {
				const char* name = mono_field_get_name(enumField);

				if (strcmp(name, "value__") == 0) continue;

				enumNames.push_back(name);
			}
		}

		std::sort(enumNames.begin(), enumNames.end());

		int currentIndex = enumValue;
		if (currentIndex < -1 || currentIndex >= enumNames.size()) {
			currentIndex = 0; 
		}

		std::string comboStr;
		for (const auto& name : enumNames) {
			comboStr += name + '\0';
		}
		comboStr += '\0'; 

		if (ImGui::Combo("##enum", &currentIndex, comboStr.c_str())) {
			mono_field_set_value(monoScript, field, &currentIndex);
		}
	}

	struct MonoVector2 {
		float X;
		float Y;
	};

	struct MonoVector3 {
		float X;
		float Y;
		float Z;
	};

	struct MonoVector4 {
		float X;
		float Y;
		float Z;
		float W;
	};

	static void DrawVector2Field(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		MonoVector2 vector;
		mono_field_get_value(monoScript, field, &vector);

		float values[2] = { vector.X, vector.Y };
		if (ImGui::DragFloat2("##vector2value", values, 0.1f)) {
			vector.X = values[0];
			vector.Y = values[1];
			mono_field_set_value(monoScript, field, &vector);
		}
	}

	static void DrawVector3Field(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		MonoVector3 vector;
		mono_field_get_value(monoScript, field, &vector);

		float values[3] = { vector.X, vector.Y, vector.Z };
		if (ImGui::DragFloat3("##vector3value", values, 0.1f)) {
			vector.X = values[0];
			vector.Y = values[1];
			vector.Z = values[2];
			mono_field_set_value(monoScript, field, &vector);
		}
	}

	static void DrawVector4Field(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		MonoVector4 vector;
		mono_field_get_value(monoScript, field, &vector);

		float values[4] = { vector.X, vector.Y, vector.Z, vector.W };
		if (ImGui::DragFloat4("##vector4value", values, 0.1f)) {
			vector.X = values[0];
			vector.Y = values[1];
			vector.Z = values[2];
			vector.W = values[3];
			mono_field_set_value(monoScript, field, &vector);
		}
	}

	static void DrawGameObjectField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		MonoObject* goFieldValue = nullptr;
		mono_field_get_value(monoScript, field, &goFieldValue);

		std::string goName = "None";
		GameObject* currentGO = nullptr;

		if (goFieldValue != nullptr) {
			MonoClass* goClass = mono_object_get_class(goFieldValue);
			MonoClassField* cppInstanceField = mono_class_get_field_from_name(goClass, "CplusplusInstance");

			if (cppInstanceField) {
				uintptr_t cppInstance = 0;
				mono_field_get_value(goFieldValue, cppInstanceField, &cppInstance);

				if (cppInstance != 0) {
					currentGO = reinterpret_cast<GameObject*>(cppInstance);
					if (currentGO) {
						goName = currentGO->GetName();
					}
				}
			}

			MonoProperty* nameProp = mono_class_get_property_from_name(goClass, "name");
			if (nameProp) {
				MonoMethod* getMethod = mono_property_get_get_method(nameProp);
				if (getMethod) {
					MonoObject* exception = nullptr;
					MonoString* nameString = (MonoString*)mono_runtime_invoke(getMethod, goFieldValue, nullptr, &exception);

					if (!exception && nameString) {
						char* name = mono_string_to_utf8(nameString);
						if (name) {
							goName = name;
							mono_free(name);
						}
					}
				}
			}
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));
		ImGui::Button(goName.c_str(), ImVec2(-1, 0));
		ImGui::PopStyleColor(2);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
				GameObject* draggedGO = *(GameObject**)payload->Data;
				if (draggedGO) {
					MonoObject* managedGO = MonoManager::GetInstance().CreateGameObjectReference(draggedGO);

					if (managedGO) {
						mono_field_set_value(monoScript, field, managedGO);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Clear Reference")) {
				void* nullRef = nullptr;
				mono_field_set_value(monoScript, field, nullRef);
			}
			ImGui::EndPopup();
		}

		if (ImGui::IsItemHovered() && currentGO) {
			ImGui::BeginTooltip();
			ImGui::Text("GameObject: %s", goName.c_str());
			ImGui::Text("ID: %d", currentGO->GetID());
			ImGui::EndTooltip();
		}
	}

	static void DrawStringField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		std::string value = MonoFieldHelper::GetStringValue(monoScript, field);
		char buffer[256];
		strcpy_s(buffer, value.c_str());

		if (ImGui::InputText("##value", buffer, IM_ARRAYSIZE(buffer))) {
			MonoFieldHelper::SetStringValue(monoScript, field, buffer);
		}
	}

	static void DrawBoolField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		bool value = false;
		mono_field_get_value(monoScript, field, &value);

		if (ImGui::Checkbox("##value", &value)) {
			mono_field_set_value(monoScript, field, &value);
		}
	}

	static void DrawIntField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		int value = 0;
		mono_field_get_value(monoScript, field, &value);

		if (ImGui::DragInt("##value", &value)) {
			mono_field_set_value(monoScript, field, &value);
		}
	}

	static void DrawFloatField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		float value = 0.0f;
		mono_field_get_value(monoScript, field, &value);

		if (ImGui::DragFloat("##value", &value)) {
			mono_field_set_value(monoScript, field, &value);
		}
	}

	static void DrawDoubleField(MonoObject* monoScript, MonoClassField* field, const char* fieldName) {
		double value = 0.0;
		mono_field_get_value(monoScript, field, &value);

		if (ImGui::InputDouble("##value", &value, 0.0, 0.0, "%.6f")) {
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

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				system->GetOwner()->RemoveComponent<ParticleFX>();
			}
			ImGui::EndPopup();
		}

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
				system->Pause();
			}
		}
		else {
			if (ImGui::Button("Play", ImVec2(width, 0))) {
				system->Play();
			}
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

		bool playOnAwake = system->GetPlayOnAwake();
		if (ImGui::Checkbox("Play on awake", &playOnAwake)) {
			system->SetPlayOnAwake(playOnAwake);
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

		//Duration 

		float duration = system->GetDuration();

		if (ImGui::DragFloat("Duration", &duration, 0.1f, 0.1f, 1000.0f)) {
			system->SetDuration(duration);
		}

		// Lifetime
		float minLifetime = system->GetMinLifetime();
		float maxLifetime = system->GetMaxLifetime();
		if (ImGui::DragFloatRange2("Lifetime", &minLifetime, &maxLifetime, 0.05f, 0.01f, 20.0f)) {
			system->SetParticleLifetime(minLifetime, maxLifetime);
		}

		// Speed
		float minSpeed = system->GetMinSpeed();
		float maxSpeed = system->GetMaxSpeed();
		if (ImGui::DragFloatRange2("Speed in a range", &minSpeed, &maxSpeed, 0.05f, 0.0f, 50.0f)) {
			system->SetParticleSpeed(minSpeed, maxSpeed);
		}

		float endSpeed = system->GetEndSpeed();

		if (ImGui::DragFloat("Set final speed", &endSpeed, 0.05f, -50.0f,50.0f)) {
			system->SetEndSpeed(endSpeed);
		}

		// Size
		float endSize = system->GetEndSize();
		if (ImGui::DragFloat("Size end", &endSize, 0.05f, 0.01f, 10.0f)) {
			system->SetParticleEndSize(endSize);
		}
	
		float minScale = system->GetMinScale();
		float maxScale = system->GetMaxScale();
		if (ImGui::DragFloatRange2("Size in a range", &minScale, &maxScale, 0.05f, 0.00f, 10.0f)) {
			system->SetMinScale(minScale);
			system->SetMaxScale(maxScale);
		}

		// Rotation
		float rotationSpeed = system->GetRotationSpeed();
		if (ImGui::DragFloat("Rotation Speed", &rotationSpeed, 0.1f, 0.0f, 10.0f)) {
			system->SetParticleRotation(rotationSpeed);
		}
		bool randomRotation = system->GetRandomRotation();
		if (ImGui::Checkbox("Random Rotation", &randomRotation)) {
			system->SetRandomRotation(randomRotation);
		}
		float startRotation = system->GetStartRotation();
		
		if (ImGui::DragFloat("Start rotation", &startRotation, 0.1f, 0.0f, 360.0f)) {
			
			system->SetStartRotation(startRotation);
		}

		// Gravity
        float gravity[3] = { system->GetGravity().x, system->GetGravity().y, system->GetGravity().z };
        if (ImGui::DragFloat3("Gravity", gravity, 0.01f, -10.0f, 10.0f)) {
        system->SetGravity(glm::vec3(gravity[0], gravity[1], gravity[2]));
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

		int particleID = system->particleID;

		if (ImGui::InputInt("Particle preset ID", &particleID)) {
			system->particleID = particleID;
		}

		if (ImGui::Button("Set particle preset", ImVec2(width, 0))) {
			system->ApplyPreset(particleID);
		}

		// Softness
		float softness = system->GetSoftness();
		if (ImGui::SliderFloat("Edge Softness", &softness, 0.0f, 1.0f)) {
			system->SetSoftness(softness);
		}

		//SpriteSheet Animation
		bool useAnimation = system->GetUseAnimation();
		if (ImGui::Checkbox("Use animation", &useAnimation)) 
		{
			system->SetUseAnimation(useAnimation);
		}

		bool randomStartIndex = system->GetRandomStartIndex();
		if (ImGui::Checkbox("Random start index", &randomStartIndex)) {
			system->SetRandomStartIndex(randomStartIndex);
		}

		glm::vec2 spriteSize = system->GetSpriteSize();
		float spriteSizeArray[2] = { spriteSize.x, spriteSize.y };
		if (ImGui::DragFloat2("Sprite Size", spriteSizeArray, 0.1f, 0.1f, 4600.0f)) {
			system->SetSpriteSize(glm::vec2(spriteSizeArray[0], spriteSizeArray[1]));
		}

		float animSpeed = system->GetAnimSpeed();
		if (ImGui::DragFloat("Animation Speed", &animSpeed, 0.1f, 0.1f, 100.0f)) {
			system->SetAnimSpeed(animSpeed);
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

		
		int selectionIdx = -1;
		
		if (ImGui::Button("Add Color Point", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			Application->gui->foo[0].x = ImGui::CurveTerminator;
		}

		if (ImGui::Curve("", ImVec2(600, 200), 10, Application->gui->foo,&selectionIdx))
		{
			// curve changed
		}

		float value_you_care_about = ImGui::CurveValue(0.7f, 10, Application->gui->foo); // calculate value at position 0.7

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

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				canvas->GetOwner()->RemoveComponent<UICanvasComponent>();
			}
			ImGui::EndPopup();
		}

        ImGui::Text("Canvas");
    }
	#pragma endregion

    #pragma region Image
	static void DrawImageComponent(UIImageComponent* image) {
		if (!image) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("Image")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				image->GetOwner()->RemoveComponent<UIImageComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;
		const float previewSize = 24.0f;

		ImGui::BeginGroup();

		auto imageTexture = image->GetTexture();

		ImGui::PushID((void*)image);
		if (imageTexture && imageTexture->id() != 0) {
			ImGui::Image((void*)(intptr_t)imageTexture->id(), ImVec2(previewSize, previewSize));

			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImVec2 constrainedSize = CalculatePreviewSize(imageTexture->width(), imageTexture->height(), 300);
				ImGui::Image((void*)(intptr_t)imageTexture->id(), constrainedSize);
				ImGui::Text("%dx%d", imageTexture->width(), imageTexture->height());
				ImGui::EndTooltip();
			}

			if (ImGui::BeginPopupContextItem("TextureContextMenu")) {
				ImGui::Text("Texture Options");
				ImGui::Separator();
				if (ImGui::MenuItem("Clear")) {
				}
				ImGui::EndPopup();
			}
		}
		else {
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRect(p, ImVec2(p.x + previewSize, p.y + previewSize), IM_COL32(180, 180, 180, 255));
			ImGui::Button("##empty", ImVec2(previewSize, previewSize));
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
				const char* path = static_cast<const char*>(payload->Data);
				std::string extension = std::filesystem::path(path).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

				const std::array<std::string, 5> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".tga" };
				if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
					image->SetTexture(path);
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();

		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Image");

		image->GetColor();
		float color[4] = { image->GetColor().r, image->GetColor().g, image->GetColor().b, image->GetColor().a };
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::ColorEdit3("##Color", color)) {
			image->SetColor({ color[0], color[1], color[2], image->GetColor().w });
		}
		ImGui::PopItemWidth();

		float alpha = image->GetColor().a;
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Alpha");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##Alpha", &alpha, 0.0f, 1.0f)) {
			image->SetColor({ color[0], color[1], color[2], alpha });
		}
		ImGui::PopItemWidth();

		bool useAnimation = image->GetUseAnimation();
		if (ImGui::Checkbox("Use Animation", &useAnimation)) {
			image->SetUseAnimation(useAnimation);
		}

		glm::vec2 spriteSize = image->GetSpriteSize();
		float spriteSizeArray[2] = { spriteSize.x, spriteSize.y };
		if (ImGui::DragFloat2("Sprite Size", spriteSizeArray, 0.1f, 0.1f, 4600.0f)) {
			image->SetSpriteSize(glm::vec2(spriteSizeArray[0], spriteSizeArray[1]));
		}

		float animSpeed = image->GetAnimSpeed();
        if (ImGui::SliderFloat("Animation Speed", &animSpeed, 0.1f, 100.0f)) {
			image->SetAnimSpeed(animSpeed);
		}

		int animNum = image->GetAnimationNum();
		if (ImGui::DragInt("Animation Num", &animNum, 1, 0, 1)) {
			image->SetAnimationNum(animNum);
		}

		int animIndexLimit = image->GetAnimationIndexLimit();
		if (ImGui::DragInt("Animation Index Limit", &animIndexLimit, 1, 1, 100)) {
			image->SetAnimationIndexLimit(animIndexLimit);
		}

		ImGui::EndGroup();
	}
	#pragma endregion

    #pragma region RectTransform
	static void DrawRectTransformComponent(UITransformComponent* transform) {
		if (!transform) return;

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (!ImGui::CollapsingHeader("RectTransform")) return;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Remove Component")) {
				transform->GetOwner()->RemoveComponent<UITransformComponent>();
			}
			ImGui::EndPopup();
		}

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float labelWidth = windowWidth * 0.4f;

        glm::dvec3 currentPosition = transform->GetPosition();
        glm::dvec3 currentRotation = glm::radians(transform->GetRotation());
        glm::dvec3 currentScale = transform->GetScale();
		glm::dvec1 currentPivot = transform->GetPivotOffset();

		float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
		float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
		float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		bool posChanged = ImGui::DragFloat3("##Position", pos, 0.001f, -1.0f, 1.0f);
		ImGui::PopItemWidth();

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("UI position (X, Y, Z)");
			ImGui::EndTooltip();
		}

		if (posChanged) {
			glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
			glm::dvec3 deltaPos = newPosition - currentPosition;
			transform->Translate(deltaPos);
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		bool rotChanged = ImGui::DragFloat3("##Rotation", rot, 0.1f);
		ImGui::PopItemWidth();

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Rotation in degrees (X, Y, Z)");
			ImGui::EndTooltip();
		}

		if (rotChanged) {
			glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
			glm::dvec3 deltaRot = newRotation - currentRotation;
			transform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
			transform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
			transform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale");
		ImGui::SameLine(labelWidth);
		ImGui::PushItemWidth(-1);
		bool scaChanged = ImGui::DragFloat3("##Scale", sca, 0.001f, 0.001f, 10.0f);
		ImGui::PopItemWidth();

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("UI scale multiplier (X, Y, Z)");
			ImGui::EndTooltip();
		}

		if (scaChanged) {
			glm::dvec3 newScale = { sca[0], sca[1], sca[2] };
			glm::dvec3 deltaScale = newScale / currentScale;
			transform->Scale(deltaScale);
		}

		/*if (ImGui::DragFloat3("Pivot", pivot, 0.001f, -1.0f, 1.0f)) {
			glm::dvec3 newPivot = { pivot[0], pivot[1], pivot[2] };
			glm::dvec3 deltaPivot = newPivot - currentPivot;
			transform->SetPivotOffset(deltaPivot);
		}*/

		ImGui::EndGroup();
	}
	#pragma endregion

public:
	#pragma region DrawComponents
	static void DrawComponents(GameObject* gameObject, bool& snap, float& snapValue) {
		if (!gameObject) return;

		static bool showComponents = true;

		ImGui::PushStyleColor(ImGuiCol_Button, showComponents ? ImVec4(0.4f, 0.4f, 0.4f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

		if (ImGui::Button("Components", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0))) {
			showComponents = true;
		}

		ImGui::PopStyleColor(3);
		ImGui::SameLine(0, 0);

		ImGui::PushStyleColor(ImGuiCol_Button, !showComponents ? ImVec4(0.4f, 0.4f, 0.4f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

		if (ImGui::Button("Scripts", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			showComponents = false;
		}

		ImGui::PopStyleColor(3);
		ImGui::Separator();

		if (showComponents) {
			DrawEngineComponents(gameObject, snap, snapValue);
		}
		else {
			if (!gameObject->scriptComponents.empty()) {
				DrawScriptComponents(gameObject);
			}
			else {
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No script components attached");
			}
		}
	}

	static void DrawEngineComponents(GameObject* gameObject, bool& snap, float& snapValue) {
		if (!gameObject) return;

		if (gameObject->HasComponent<Transform_Component>() && !gameObject->HasComponent<UITransformComponent>()) {
			Transform_Component* transform = gameObject->GetComponent<Transform_Component>();
			DrawTransformComponent(transform, snap, snapValue);
		}

		if (gameObject->HasComponent<UITransformComponent>()) {
			UITransformComponent* uiTransformComponent = gameObject->GetComponent<UITransformComponent>();
			DrawRectTransformComponent(uiTransformComponent);
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

		if (gameObject->HasComponent<BoxColliderComponent>()) {
			BoxColliderComponent* collider = gameObject->GetComponent<BoxColliderComponent>();
			DrawColliderComponent(collider);
		}

		if (gameObject->HasComponent<MeshColliderComponent>()) {
			MeshColliderComponent* meshCollider = gameObject->GetComponent<MeshColliderComponent>();
			DrawMeshColliderComponent(meshCollider);
		}

		if (gameObject->HasComponent<CapsuleColliderComponent>()) {
			CapsuleColliderComponent* capsuleCollider = gameObject->GetComponent<CapsuleColliderComponent>();
			DrawCapsuleColliderComponent(capsuleCollider);
		}

		if (gameObject->HasComponent<RigidbodyComponent>()) {
			RigidbodyComponent* rigidbody = gameObject->GetComponent<RigidbodyComponent>();
			DrawRigidbodyComponent(rigidbody);
		}

		if (gameObject->HasComponent<ParticleFX>()) {
			ParticleFX* emitter = gameObject->GetComponent<ParticleFX>();
			DrawParticleSystemComponent(emitter);
		}
		
		if (gameObject->HasComponent<UICanvasComponent>()) {
			UICanvasComponent* uiCanvasComponent = gameObject->GetComponent<UICanvasComponent>();
			DrawCanvasComponent(uiCanvasComponent);
		}

		if (gameObject->HasComponent<UIImageComponent>()) {
			UIImageComponent* uiImageComponent = gameObject->GetComponent<UIImageComponent>();
			DrawImageComponent(uiImageComponent);
		}
	}

	static void DrawScriptsTab(GameObject* gameObject) {
		if (gameObject->scriptComponents.empty()) {
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No script components attached");
			return;
		}

		DrawScriptComponents(gameObject);
	}
	#pragma endregion

	#pragma region AddComponentMenu
	static void DrawAddComponentButton(GameObject* gameObject) {
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			ImGui::OpenPopup("AddComponentMenu");
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(350, 450), ImGuiCond_Appearing);

		static bool inCategoryView = true;
		static int currentCategory = 0;
		static char searchBuffer[64] = "";

		if (ImGui::BeginPopupModal("AddComponentMenu", nullptr, ImGuiWindowFlags_NoCollapse)) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			bool searchEdited = ImGui::InputTextWithHint("##ComponentSearch", "Search...", searchBuffer, sizeof(searchBuffer));
			ImGui::PopStyleVar();

			ImGui::Separator();

			if (searchBuffer[0] != '\0') {
				inCategoryView = false;
				DrawSearchResults(gameObject, searchBuffer);
			}
			else {
				if (inCategoryView) {
					DrawCategoryList(inCategoryView, currentCategory);
				}
				else {
					if (ImGui::Button("Back to Categories")) {
						inCategoryView = true;
					}

					ImGui::Separator();

					const char* categoryTitle = GetCategoryTitle(currentCategory);
					ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(categoryTitle).x) * 0.5f);
					ImGui::Text("%s", categoryTitle);

					ImGui::Separator();

					DrawCategoryComponents(gameObject, currentCategory);
				}
			}

			ImGui::Separator();

			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 120) * 0.5f);
			if (ImGui::Button("Close", ImVec2(120, 0))) {
				inCategoryView = true;
				searchBuffer[0] = '\0';
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	static void DrawCategoryList(bool& inCategoryView, int& currentCategory) {
		const char* categories[] = {
			"Rendering",
			"Physics",
			"Audio",
			"UI",
			"Effects",
			"Scripts",
			"Input",
			"Navigation"
		};

		for (int i = 0; i < IM_ARRAYSIZE(categories); i++) {
			bool hovered = false;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.40f, 0.70f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.40f, 0.70f, 1.0f));

			ImGui::PushID(i);
			ImGui::Button(categories[i], ImVec2(ImGui::GetContentRegionAvail().x, 35));
			hovered = ImGui::IsItemHovered();

			if (hovered && ImGui::IsMouseClicked(0)) {
				currentCategory = i;
				inCategoryView = false;
			}

			ImVec2 buttonMin = ImGui::GetItemRectMin();
			ImVec2 buttonMax = ImGui::GetItemRectMax();
			ImVec2 textPos = ImVec2(buttonMax.x - 20, buttonMin.y + (buttonMax.y - buttonMin.y) * 0.5f - ImGui::GetTextLineHeight() * 0.5f);
			ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), ">");

			ImGui::PopID();
			ImGui::PopStyleColor(3);
		}
	}

	static void DrawCategoryComponents(GameObject* gameObject, int category) {
		switch (category) {
		case 0:
			DrawComponentButton(gameObject, "Camera", [gameObject]() {
				gameObject->AddComponent<CameraComponent>();
				}, !gameObject->HasComponent<CameraComponent>());

			DrawComponentButton(gameObject, "Mesh Renderer", [gameObject]() {
				Application->root->AddMeshRenderer(*gameObject, Mesh::CreateCube(), "Assets/default.png");
				}, !gameObject->HasComponent<MeshRenderer>());

			DrawComponentButton(gameObject, "Light", [gameObject]() {
				gameObject->AddComponent<LightComponent>();
				}, !gameObject->HasComponent<LightComponent>());

			DrawComponentButton(gameObject, "Shader", [gameObject]() {
				gameObject->AddComponent<ShaderComponent>();
				}, !gameObject->HasComponent<ShaderComponent>());

			break;

		case 1:
			DrawComponentButton(gameObject, "Box Collider", [gameObject]() {
				gameObject->AddComponent<BoxColliderComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<BoxColliderComponent>() &&
					!gameObject->HasComponent<CapsuleColliderComponent>() &&
					!gameObject->HasComponent<MeshColliderComponent>());

			DrawComponentButton(gameObject, "Capsule Collider", [gameObject]() {
				gameObject->AddComponent<CapsuleColliderComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<BoxColliderComponent>() &&
					!gameObject->HasComponent<CapsuleColliderComponent>() &&
					!gameObject->HasComponent<MeshColliderComponent>());

			DrawComponentButton(gameObject, "Mesh Collider", [gameObject]() {
				gameObject->AddComponent<MeshColliderComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<BoxColliderComponent>() &&
					!gameObject->HasComponent<CapsuleColliderComponent>() &&
					!gameObject->HasComponent<MeshColliderComponent>());

			DrawComponentButton(gameObject, "Rigidbody", [gameObject]() {
				gameObject->AddComponent<RigidbodyComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<RigidbodyComponent>());

			break;

		case 2:
			DrawComponentButton(gameObject, "Sound", [gameObject]() {
				gameObject->AddComponent<SoundComponent>();
				}, !gameObject->HasComponent<SoundComponent>());

			DrawComponentButton(gameObject, "Audio Listener", [gameObject]() {
				gameObject->AddComponent<AudioListener>();
				}, !gameObject->HasComponent<AudioListener>());

			break;

		case 3:
			DrawComponentButton(gameObject, "Canvas", [gameObject]() {
				if (!gameObject->HasComponent<UITransformComponent>()) {
					gameObject->AddComponent<UITransformComponent>();
				}
				gameObject->AddComponent<UICanvasComponent>();
				}, !gameObject->HasComponent<UICanvasComponent>());

			DrawComponentButton(gameObject, "Image", [gameObject]() {
				if (!gameObject->HasComponent<UITransformComponent>()) {
					gameObject->AddComponent<UITransformComponent>();
				}
				gameObject->AddComponent<UIImageComponent>();
				gameObject->GetComponent<UIImageComponent>()->SetTexture("Assets/default.png");
				}, !gameObject->HasComponent<UIImageComponent>());

			DrawComponentButton(gameObject, "Rect Transform", [gameObject]() {
				gameObject->AddComponent<UITransformComponent>();
				}, !gameObject->HasComponent<UITransformComponent>());

			break;

		case 4:
			DrawComponentButton(gameObject, "Particle System", [gameObject]() {
				gameObject->AddComponent<ParticleFX>();
				}, !gameObject->HasComponent<ParticleFX>());

			DrawComponentButton(gameObject, "Skeletal Animation", [gameObject]() {
				gameObject->AddComponent<SkeletalAnimationComponent>();
				}, !gameObject->HasComponent<SkeletalAnimationComponent>());

			break;

		case 5:
			DrawAddScriptComponents(gameObject);
			break;

		case 6:
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No input components available");
			break;

		case 7:
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No navigation components available");
			break;
		}
	}

	static void DrawAddScriptComponents(GameObject* gameObject) {
		std::vector<std::string> availableScripts;
		try {
			availableScripts = MonoManager::GetInstance().scriptNames;
		}
		catch (...) {
			availableScripts = { "PlayerController", "EnemyAI", "ItemCollector", "CameraFollower" };
		}

		for (const auto& scriptName : availableScripts) {
			DrawComponentButton(gameObject, scriptName.c_str(), [gameObject, scriptName]() {
				gameObject->AddComponent<ScriptComponent>()->LoadScript(scriptName);
				}, true);
		}

		ImGui::Separator();
		if (ImGui::Button("Add New Script", ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
			ImGui::OpenPopup("AddCustomScript");
		}

		static char scriptNameBuffer[64] = "";
		if (ImGui::BeginPopupModal("AddCustomScript", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Enter the name of the script to add:");
			ImGui::InputText("##ScriptName", scriptNameBuffer, sizeof(scriptNameBuffer));

			ImGui::Separator();

			if (ImGui::Button("Add", ImVec2(120, 0))) {
				if (strlen(scriptNameBuffer) > 0) {
					MonoManager::GetInstance().CreateNewScript(scriptNameBuffer);
					gameObject->AddComponent<ScriptComponent>()->LoadScript(scriptNameBuffer);
					scriptNameBuffer[0] = '\0';
					ImGui::ClosePopupToLevel(0, true);
				}

			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				scriptNameBuffer[0] = '\0';
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	static void DrawSearchResults(GameObject* gameObject, const char* searchStr) {
		std::string searchLower = searchStr;
		std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

		bool anyFound = false;

		auto matchesSearch = [&searchLower](const char* name) {
			std::string nameLower = name;
			std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
			return nameLower.find(searchLower) != std::string::npos;
			};

		if (matchesSearch("Camera")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Camera", [gameObject]() {
				gameObject->AddComponent<CameraComponent>();
				}, !gameObject->HasComponent<CameraComponent>());
		}

		if (matchesSearch("Mesh Renderer")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Mesh Renderer", [gameObject]() {
				Application->root->AddMeshRenderer(*gameObject, Mesh::CreateCube(), "Assets/default.png");
				}, !gameObject->HasComponent<MeshRenderer>());
		}

		if (matchesSearch("Light")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Light", [gameObject]() {
				gameObject->AddComponent<LightComponent>();
				}, !gameObject->HasComponent<LightComponent>());
		}

		if (matchesSearch("Box Collider")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Box Collider", [gameObject]() {
				gameObject->AddComponent<BoxColliderComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<BoxColliderComponent>() &&
					!gameObject->HasComponent<CapsuleColliderComponent>() &&
					!gameObject->HasComponent<MeshColliderComponent>());
		}

		if (matchesSearch("Capsule Collider")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Capsule Collider", [gameObject]() {
				gameObject->AddComponent<CapsuleColliderComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<BoxColliderComponent>() &&
					!gameObject->HasComponent<CapsuleColliderComponent>() &&
					!gameObject->HasComponent<MeshColliderComponent>());
		}

		if (matchesSearch("Mesh Collider")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Mesh Collider", [gameObject]() {
				gameObject->AddComponent<MeshColliderComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<BoxColliderComponent>() &&
					!gameObject->HasComponent<CapsuleColliderComponent>() &&
					!gameObject->HasComponent<MeshColliderComponent>());
		}

		if (matchesSearch("Rigidbody")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Rigidbody", [gameObject]() {
				gameObject->AddComponent<RigidbodyComponent>(Application->physicsModule);
				}, !gameObject->HasComponent<RigidbodyComponent>());
		}

		if (matchesSearch("Sound")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Sound", [gameObject]() {
				gameObject->AddComponent<SoundComponent>();
				}, !gameObject->HasComponent<SoundComponent>());
		}

		if (matchesSearch("Audio Listener")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Audio Listener", [gameObject]() {
				gameObject->AddComponent<AudioListener>();
				}, !gameObject->HasComponent<AudioListener>());
		}

		if (matchesSearch("Canvas")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Canvas", [gameObject]() {
				if (!gameObject->HasComponent<UITransformComponent>()) {
					gameObject->AddComponent<UITransformComponent>();
				}
				gameObject->AddComponent<UICanvasComponent>();
				}, !gameObject->HasComponent<UICanvasComponent>());
		}

		if (matchesSearch("UIImageComponent")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Image", [gameObject]() {
				if (!gameObject->HasComponent<UITransformComponent>()) {
					gameObject->AddComponent<UITransformComponent>();
				}
				gameObject->AddComponent<UIImageComponent>();
				gameObject->GetComponent<UIImageComponent>()->SetTexture("Assets/default.png");
				}, !gameObject->HasComponent<UIImageComponent>());
		}

		if (matchesSearch("Rect Transform")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Rect Transform", [gameObject]() {
				gameObject->AddComponent<UITransformComponent>();
				}, !gameObject->HasComponent<UITransformComponent>());
		}

		if (matchesSearch("Particle") || matchesSearch("System")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Particle System", [gameObject]() {
				gameObject->AddComponent<ParticleFX>();
				}, !gameObject->HasComponent<ParticleFX>());
		}

		if (matchesSearch("Skeletal") || matchesSearch("Animation")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Skeletal Animation", [gameObject]() {
				gameObject->AddComponent<SkeletalAnimationComponent>();
				}, !gameObject->HasComponent<SkeletalAnimationComponent>());
		}

		if (matchesSearch("Shader")) {
			anyFound = true;
			DrawComponentButton(gameObject, "Shader", [gameObject]() {
				gameObject->AddComponent<ShaderComponent>();
				}, !gameObject->HasComponent<ShaderComponent>());
		}

		std::vector<std::string> availableScripts;
		try {
			availableScripts = MonoManager::GetInstance().scriptNames;
		}
		catch (...) {
			availableScripts = { "PlayerController", "EnemyAI", "ItemCollector", "CameraFollower" };
		}

		for (const auto& scriptName : availableScripts) {
			if (matchesSearch(scriptName.c_str())) {
				anyFound = true;
				DrawComponentButton(gameObject, scriptName.c_str(), [gameObject, scriptName]() {
					gameObject->AddComponent<ScriptComponent>()->LoadScript(scriptName);
					}, true);
			}
		}

		if (!anyFound) {
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
				"No components match your search criteria");
		}
	}

	static void DrawComponentButton(GameObject* gameObject, const char* name, std::function<void()> addAction, bool available) {
		ImGui::PushID(name);

		ImVec4 buttonColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		ImVec4 buttonHoveredColor = ImVec4(0.25f, 0.40f, 0.70f, 1.0f);
		ImVec4 buttonActiveColor = ImVec4(0.25f, 0.40f, 0.70f, 1.0f);
		ImVec4 textColor = available ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);

		bool clicked = ImGui::Button(name, ImVec2(ImGui::GetContentRegionAvail().x, 35));

		ImGui::PopStyleColor(4);

		if (clicked && available) {
			addAction();
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopID();
	}

	static const char* GetCategoryTitle(int category) {
		const char* titles[] = {
			"Rendering Components",
			"Physics Components",
			"Audio Components",
			"UI Components",
			"Effects Components",
			"Scripts",
			"Input Components",
			"Navigation Components"
		};

		if (category >= 0 && category < IM_ARRAYSIZE(titles)) {
			return titles[category];
		}

		return "Components";
	}
#pragma endregion

private:
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

	ImGui::Separator();
	ImGui::Text("Tag:");

	const std::vector<std::string>& tags = SceneManagement->GetTags();
	int selectedTagIndex = -1;

	std::string currentTag = gameObject->GetTag();
	for (size_t i = 0; i < tags.size(); ++i) {
		if (tags[i] == currentTag) {
			selectedTagIndex = static_cast<int>(i);
			break;
		}
	}

	if (ImGui::BeginCombo("##GameObjectTag", currentTag.c_str())) {
		for (size_t i = 0; i < tags.size(); ++i) {
			bool isSelected = (selectedTagIndex == static_cast<int>(i));
			if (ImGui::Selectable(tags[i].c_str(), isSelected)) {
				gameObject->SetTag(tags[i]);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	static char newTag[128] = {};
	ImGui::InputText("New Tag", newTag, sizeof(newTag));

	if (ImGui::Button("Add Tag") && strlen(newTag) > 0) {
		std::string newTagStr(newTag);
		SceneManagement->AddTag(newTagStr);
		gameObject->SetTag(newTagStr);
		memset(newTag, 0, sizeof(newTag));  
	}
}