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
#include "../MyScriptingEngine/ScriptComponent.h"
#include "../MyScriptingEngine/MonoManager.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "../MyParticlesEngine/ParticlesEmitterComponent.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "../MyUIEngine/UIImageComponent.h"

typedef unsigned int guint32;
#pragma endregion

class ComponentDrawer {
private:
    #pragma region Transform
    static void DrawTransformComponent(Transform_Component* transform, bool& snap, float& snapValue) {
        if (!transform) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Transform")) return;

        glm::dvec3 currentPosition = transform->GetPosition();
        glm::dvec3 currentRotation = glm::radians(transform->GetEulerAngles());
        glm::dvec3 currentScale = transform->GetScale();

        float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
        float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
        float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

        if (ImGui::DragFloat3("Position", pos, 0.1f)) {
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
    #pragma endregion

    #pragma region MeshRenderer
    static void DrawMeshProperties(std::shared_ptr<Mesh> mesh) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Mesh")) return;

        ImGui::Text("Vertices: %d", mesh->getModel()->GetModelData().vertexData.size());
        ImGui::Text("Indices: %d", mesh->getModel()->GetModelData().indexData.size());

        bool& triNormals = mesh->drawTriangleNormals;
        bool& vertexNormals = mesh->drawVertexNormals;

        ImGui::Checkbox("Triangle Normals", &triNormals);
        ImGui::Checkbox("Vertex Normals", &vertexNormals);
    }

    static void DrawTexturePreview(std::shared_ptr<Image> image) {
        ImGui::Text("Preview:");
        ImVec2 imageSize = CalculatePreviewSize(image->width(), image->height());
        ImGui::Image((void*)(intptr_t)image->id(), imageSize);
    }

    static void DrawColorPicker(MeshRenderer* meshRenderer) {
        vec4 matColor = meshRenderer->GetMaterial()->GetColor();
        float colorArray[4] = {
            static_cast<float>(matColor.x),
            static_cast<float>(matColor.y),
            static_cast<float>(matColor.z),
            static_cast<float>(matColor.w)
        };

        if (ImGui::ColorPicker4("Color", colorArray)) {
            vec4 newColor(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
            meshRenderer->GetMaterial()->SetColor(newColor);
        }
    }

    static void DrawMaterialProperties(MeshRenderer* meshRenderer, std::shared_ptr<Image> image) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Material")) return;

        ImGui::Text("Dimensions: %dx%d", image->width(), image->height());

        auto textureID = image->id();
        if (textureID) {
            DrawTexturePreview(image);
            DrawColorPicker(meshRenderer);
        }
    }

    static void DrawMeshRendererComponent(MeshRenderer* meshRenderer) {
        if (!meshRenderer) return;

        std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh();
        if (mesh) {
            DrawMeshProperties(mesh);
        }

        std::shared_ptr<Image> image = meshRenderer->GetMaterial()->getImg();
        if (image) {
            DrawMaterialProperties(meshRenderer, image);
        }
    }
    #pragma endregion

    #pragma region Camera
    static void DrawCameraComponent(CameraComponent* camera) {
        if (!camera) return;

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (!ImGui::CollapsingHeader("Camera")) return;

        bool orthographic = camera->IsOrthographic();
        bool frustum = camera->frustrumCullingEnabled;
        float orthoSize = camera->GetOrthoSize();
        float fov = camera->GetFOV();
        float nearPlane = static_cast<float>(camera->GetNearPlane());
        float farPlane = static_cast<float>(camera->GetFarPlane());

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

        if (ImGui::Combo("Shader Type", &shaderType, "Default\0Light\0Water\0")) {
            shader->SetShaderType(static_cast<ShaderType>(shaderType));
        }

        if (currentType == ShaderType::WATER) {
            ImGui::DragFloat("Amplitude", &shader->amplitude, 0.1f, 0.1f, 10.0f);
            ImGui::DragFloat("Frequency", &shader->frequency, 0.1f, 0.1f, 10.0f);
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

            HINSTANCE result = ShellExecuteA(NULL, "open", scriptPath.c_str(), NULL, NULL, SW_SHOW);
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

    #pragma region Particles
    static void DrawParticleEmitterComponent(ParticlesEmitterComponent* emitter) {
        if (!emitter) return;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        bool isOpen = ImGui::CollapsingHeader("Particle Emitter", ImGuiTreeNodeFlags_DefaultOpen);
        ImGui::PopStyleVar();

        if (!isOpen) return;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
        ImGui::Indent(10.0f);

        DrawEmissionSettings(emitter);
        DrawRenderingSettings(emitter);

        ImGui::Unindent(10.0f);
        ImGui::PopStyleVar();
    }

    static void DrawEmissionSettings(ParticlesEmitterComponent* emitter) {
        if (!ImGui::TreeNodeEx("Emission Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

        ImGui::BeginGroup();

        float spawnRate = emitter->getSpawnRate();
        if (ImGui::DragFloat("Spawn Rate##spawner", &spawnRate, 0.1f, 0.1f, 100.0f, "%.1f")) {
            emitter->setSpawnRate(spawnRate);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Number of particles spawned per second");
        }

        int maxParticles = emitter->getMaxParticles();
        if (ImGui::DragInt("Max Particles##count", &maxParticles, 1, 1, 1000)) {
            emitter->setMaxParticles(maxParticles);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Maximum number of particles allowed at once");
        }

        ImGui::EndGroup();
        ImGui::TreePop();
    }

    static void DrawRenderingSettings(ParticlesEmitterComponent* emitter) {
        if (!ImGui::TreeNodeEx("Rendering Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

        ImGui::BeginGroup();

        DrawBillboardSettings(emitter);
        DrawTextureSettings(emitter);

        ImGui::EndGroup();
        ImGui::TreePop();
    }

    static void DrawBillboardSettings(ParticlesEmitterComponent* emitter) {
        BillboardType billboardType = emitter->GetTypeEnum();
        const char* billboardItems[] = {
            "Screen Aligned - Always faces camera",
            "World Aligned - Maintains up vector",
            "Axis Aligned - Rotates around specified axis"
        };

        if (ImGui::Combo("Billboard Type##alignment", (int*)&billboardType, billboardItems, IM_ARRAYSIZE(billboardItems))) {
            emitter->SetType(billboardType);
        }
    }

    static void DrawTextureSettings(ParticlesEmitterComponent* emitter) {
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

        std::string texturePath = emitter->GetTexture();
        char texturePathBuffer[256];
        strncpy_s(texturePathBuffer, sizeof(texturePathBuffer), texturePath.c_str(), _TRUNCATE);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        if (ImGui::InputText("##TexturePath", texturePathBuffer, sizeof(texturePathBuffer))) {
            emitter->SetTexture(texturePathBuffer);
        }
        ImGui::PopStyleVar();

        if (!texturePath.empty()) {
            ImGui::SameLine();
            if (ImGui::Button("Preview##texture")) {
                //preview here like meshrenderer component does
            }
        }

        ImGui::Button("Drop Texture Here", ImVec2(ImGui::GetContentRegionAvail().x, 30));
        HandleParticleTextureDrop(emitter);
    }

    static void HandleParticleTextureDrop(ParticlesEmitterComponent* emitter) {
        if (!ImGui::BeginDragDropTarget()) return;

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
            const char* path = static_cast<const char*>(payload->Data);
            std::string extension = std::filesystem::path(path).extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            const std::array<std::string, 5> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".image" };

            if (std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end()) {
                emitter->SetTexture(path);
            }
            else {
                LOG(LogType::LOG_WARNING, "Invalid texture format: %s", extension.c_str());
            }
        }

        ImGui::EndDragDropTarget();
    }

    //more particle settings
    /*static void DrawParticleLifetimeSettings(ParticlesEmitterComponent* emitter) {
        if (!ImGui::TreeNodeEx("Lifetime Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

        ImGui::BeginGroup();

        float lifetime = emitter->getParticleLifetime();
        if (ImGui::DragFloat("Lifetime", &lifetime, 0.1f, 0.1f, 10.0f, "%.1f")) {
            emitter->setParticleLifetime(lifetime);
        }

        float lifetimeVariance = emitter->getLifetimeVariance();
        if (ImGui::DragFloat("Lifetime Variance", &lifetimeVariance, 0.1f, 0.0f, 5.0f, "%.1f")) {
            emitter->setLifetimeVariance(lifetimeVariance);
        }

        ImGui::EndGroup();
        ImGui::TreePop();
    }

    static void DrawParticleColorSettings(ParticlesEmitterComponent* emitter) {
        if (!ImGui::TreeNodeEx("Color Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

        ImGui::BeginGroup();

        vec4 startColor = emitter->getStartColor();
        float startColorArray[4] = {
            static_cast<float>(startColor.x),
            static_cast<float>(startColor.y),
            static_cast<float>(startColor.z),
            static_cast<float>(startColor.w)
        };

        if (ImGui::ColorEdit4("Start Color", startColorArray)) {
            emitter->setStartColor(vec4(
                startColorArray[0],
                startColorArray[1],
                startColorArray[2],
                startColorArray[3]
            ));
        }

        vec4 endColor = emitter->getEndColor();
        float endColorArray[4] = {
            static_cast<float>(endColor.x),
            static_cast<float>(endColor.y),
            static_cast<float>(endColor.z),
            static_cast<float>(endColor.w)
        };

        if (ImGui::ColorEdit4("End Color", endColorArray)) {
            emitter->setEndColor(vec4(
                endColorArray[0],
                endColorArray[1],
                endColorArray[2],
                endColorArray[3]
            ));
        }

        ImGui::EndGroup();
        ImGui::TreePop();
    }

    static void DrawParticleVelocitySettings(ParticlesEmitterComponent* emitter) {
        if (!ImGui::TreeNodeEx("Velocity Settings", ImGuiTreeNodeFlags_DefaultOpen)) return;

        ImGui::BeginGroup();

        vec3 startVelocity = emitter->getStartVelocity();
        float velocityArray[3] = {
            static_cast<float>(startVelocity.x),
            static_cast<float>(startVelocity.y),
            static_cast<float>(startVelocity.z)
        };

        if (ImGui::DragFloat3("Initial Velocity", velocityArray, 0.1f)) {
            emitter->setStartVelocity(vec3(
                velocityArray[0],
                velocityArray[1],
                velocityArray[2]
            ));
        }

        float velocityVariance = emitter->getVelocityVariance();
        if (ImGui::DragFloat("Velocity Variance", &velocityVariance, 0.1f, 0.0f, 10.0f)) {
            emitter->setVelocityVariance(velocityVariance);
        }

        ImGui::EndGroup();
        ImGui::TreePop();
    }*/
    #pragma endregion

    //Aqui mas componentes

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
        
        if (gameObject->HasComponent<RigidbodyComponent>()) {
            RigidbodyComponent* rigidbody = gameObject->GetComponent<RigidbodyComponent>();
			DrawRigidbodyComponent(rigidbody);
		}

		if (gameObject->HasComponent<ParticlesEmitterComponent>()) {
			ParticlesEmitterComponent* emitter = gameObject->GetComponent<ParticlesEmitterComponent>();
			DrawParticleEmitterComponent(emitter);
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
        
        if (!gameObject->HasComponent<RigidbodyComponent>()) {
			if (ImGui::MenuItem("RigidBody")) {
				gameObject->AddComponent<RigidbodyComponent>(Application->physicsModule);
				gameObject->GetComponent<RigidbodyComponent>()->Start();
			}
		}

		if (!gameObject->HasComponent<ParticlesEmitterComponent>()) {
			if (ImGui::MenuItem("ParticleEmitter")) {
				gameObject->AddComponent<ParticlesEmitterComponent>();
			}
		}

		//Aqui mas componentes
	}

    static ImVec2 CalculatePreviewSize(int width, int height) {
        const float maxPreviewSize = 200.0f;
        float aspectRatio = static_cast<float>(width) / height;

        if (width > maxPreviewSize || height > maxPreviewSize) {
            if (aspectRatio > 1.0f) {
                return ImVec2(maxPreviewSize, maxPreviewSize / aspectRatio);
            }
            return ImVec2(maxPreviewSize * aspectRatio, maxPreviewSize);
        }

        return ImVec2(static_cast<float>(width), static_cast<float>(height));
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
}