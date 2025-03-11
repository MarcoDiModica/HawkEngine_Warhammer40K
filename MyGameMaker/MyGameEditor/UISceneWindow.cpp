#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "UISceneWindow.h"
#include "UIInspector.h"
#include "App.h"
#include "MyGUI.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "Input.h"

enum class ManipulationOperation { IDLE, TRANSLATE, ROTATE, SCALE };
enum class TransformSpace { LOCAL, WORLD };

UISceneWindow::UISceneWindow(UIType type, std::string name) : UIElement(type, name)
{
    m_World.LoadTextureLocalPath("EngineAssets/global.png");
    m_Local.LoadTextureLocalPath("EngineAssets/local.png");
    m_Trans.LoadTextureLocalPath("EngineAssets/trans.png");
    m_Rot.LoadTextureLocalPath("EngineAssets/rot.png");
    m_Sca.LoadTextureLocalPath("EngineAssets/sca.png");
    m_Setting.LoadTextureLocalPath("EngineAssets/settings.png");
}

UISceneWindow::~UISceneWindow()
{
}

void UISceneWindow::Init()
{

    glGenFramebuffers(1, &Application->gui->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);

    glGenTextures(1, &Application->gui->fboTexture);
    glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTexture, 0);

    glGenRenderbuffers(1, &Application->gui->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiConfigFlags_DockingEnable;

glm::vec3 UISceneWindow::ConvertMouseToWorldCoords(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y)
{
    int adjusted_mouse_x = mouse_x - window_x;
    int adjusted_mouse_y = mouse_y - window_y;

    float ndc_x = (2.0f * adjusted_mouse_x) / screen_width - 1.0f;
    float ndc_y = 1.0f - (2.0f * adjusted_mouse_y) / screen_height;

    glm::vec4 clip_coords = glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);
    glm::mat4 projection_matrix = Application->camera->projection();
    glm::vec4 view_coords = glm::inverse(projection_matrix) * clip_coords;
    view_coords = glm::vec4(view_coords.x, view_coords.y, -1.0f, 0.0f);

    glm::mat4 view_matrix = Application->camera->view();
    glm::vec4 world_coords = glm::inverse(view_matrix) * view_coords;

    glm::vec3 camPos = glm::vec3(
        static_cast<float>(Application->camera->GetTransform().GetPosition().x),
        static_cast<float>(Application->camera->GetTransform().GetPosition().y),
        static_cast<float>(Application->camera->GetTransform().GetPosition().z)
    );
    return glm::vec3(world_coords) + camPos;
}

glm::vec3 UISceneWindow::GetMousePickDir(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y)
{
    int adjusted_mouse_x = mouse_x - window_x;
    int adjusted_mouse_y = mouse_y - window_y;

    glm::vec3 window_coords = glm::vec3(adjusted_mouse_x, screen_height - adjusted_mouse_y, 0.0f);
    glm::mat4 view_matrix = Application->camera->view();
    glm::mat4 projection_matrix = Application->camera->projection();

    glm::vec4 viewport = glm::vec4(0, 0, screen_width, screen_height);
    glm::vec3 v0 = glm::unProject(window_coords, view_matrix, projection_matrix, viewport);
    glm::vec3 v1 = glm::unProject(glm::vec3(window_coords.x, window_coords.y, 1.0f), view_matrix, projection_matrix, viewport);
    glm::vec3 world_coords = (v1 - v0);

    return world_coords;
}

bool UISceneWindow::CheckRayAABBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const BoundingBox& bBox, glm::vec3& collisionPoint) {
    float tmin = (bBox.min.x - rayOrigin.x) / rayDir.x;
    float tmax = (bBox.max.x - rayOrigin.x) / rayDir.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (bBox.min.y - rayOrigin.y) / rayDir.y;
    float tymax = (bBox.max.y - rayOrigin.y) / rayDir.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (bBox.min.z - rayOrigin.z) / rayDir.z;
    float tzmax = (bBox.max.z - rayOrigin.z) / rayDir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    float t = tmin;
    if (t < 0) {
        t = tmax;
        if (t < 0) return false;
    }

    collisionPoint = rayOrigin + t * rayDir;
    return true;
}

bool UISceneWindow::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    
    ImGui::Begin("Scene", nullptr, windowFlags);
    {
        bool isViewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();

        winPos = vec2(windowPos.x, windowPos.y);

        if (winSize.x != contentRegionSize.x || winSize.y != contentRegionSize.y)
        {
            winSize = vec2(contentRegionSize.x, contentRegionSize.y);

            glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)winSize.x, (int)winSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)winSize.x, (int)winSize.y);

            Application->camera->UpdateCameraView(winSize.x, winSize.y, winSize.x, winSize.y);

            Application->gui->camSize = winSize;
        }

        ImRect titleBarRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + ImGui::GetFrameHeight()));

        ImVec2 mousePos = ImGui::GetMousePos();

        bool isMouseOverTitleBar = (mousePos.x >= titleBarRect.Min.x && mousePos.x <= titleBarRect.Max.x &&
            mousePos.y >= titleBarRect.Min.y && mousePos.y <= titleBarRect.Max.y);

        if (isMouseOverTitleBar) {
            windowFlags &= ~ImGuiWindowFlags_NoMove;
        }
        else {
            windowFlags |= ImGuiWindowFlags_NoMove;
        }

        ImVec2 viewportPosition = ImGui::GetCursorScreenPos();

        ImGui::Image((ImTextureID)(uintptr_t)Application->gui->fboTexture, contentRegionSize, ImVec2(0, 1), ImVec2(1, 0));

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(viewportPosition.x, viewportPosition.y, contentRegionSize.x, contentRegionSize.y);

        glm::mat4 viewMatrix = Application->camera->view();
        glm::mat4 projectionMatrix = Application->camera->projection();

        auto setActiveButtonColor = [](bool isActive) {
            if (isActive) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.53f, 0.81f, 0.92f, 0.75f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.31f, 0.26f, 0.71f, 1.0f));
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 0.75f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
            }
            };

        ImGui::SetCursorScreenPos(ImVec2(viewportPosition.x + 190, viewportPosition.y + 15));
        static TransformSpace transformSpace = TransformSpace::LOCAL;

        setActiveButtonColor(transformSpace == TransformSpace::WORLD);
        if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_World.id())), ImVec2(15, 15))) {
            transformSpace = (transformSpace == TransformSpace::LOCAL) ? TransformSpace::WORLD : TransformSpace::LOCAL;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, iconSpacing);
        setActiveButtonColor(transformSpace == TransformSpace::LOCAL);
        if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Local.id())), ImVec2(15, 15))) {
            transformSpace = (transformSpace == TransformSpace::WORLD) ? TransformSpace::LOCAL : TransformSpace::WORLD;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, iconSpacing);
        ImGui::Text(transformSpace == TransformSpace::LOCAL ? "local" : "global");
        ImGui::SetCursorScreenPos(ImVec2(viewportPosition.x + 20, viewportPosition.y + 10));

        static ManipulationOperation operation = ManipulationOperation::IDLE;

        setActiveButtonColor(operation == ManipulationOperation::TRANSLATE);
        if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Trans.id())), ImVec2(iconSize, iconSize))) {
            operation = (operation == ManipulationOperation::TRANSLATE) ? ManipulationOperation::IDLE : ManipulationOperation::TRANSLATE;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, iconSpacing);

        setActiveButtonColor(operation == ManipulationOperation::ROTATE);
        if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Rot.id())), ImVec2(iconSize, iconSize))) {
            operation = (operation == ManipulationOperation::ROTATE) ? ManipulationOperation::IDLE : ManipulationOperation::ROTATE;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, iconSpacing);

        setActiveButtonColor(operation == ManipulationOperation::SCALE);
        if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Sca.id())), ImVec2(iconSize, iconSize))) {
            operation = (operation == ManipulationOperation::SCALE) ? ManipulationOperation::IDLE : ManipulationOperation::SCALE;
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine(0, iconSpacing);
        setActiveButtonColor(false);
        if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Setting.id())), ImVec2(iconSize, iconSize))) {
            ImGui::OpenPopup("CameraConfigPopup");
        }
        ImGui::PopStyleColor(2);

        if (ImGui::BeginPopup("CameraConfigPopup")) {
            ImGui::Text("Camera Settings");
            ImGui::Separator();

            static int msaaSamples = 4;
            const char* msaaOptions[] = { "Off", "2x", "4x", "8x" };
            if (ImGui::Combo("Antialiasing", &msaaSamples, msaaOptions, IM_ARRAYSIZE(msaaOptions))) {
                int actualSamples = (msaaSamples == 0) ? 0 : (1 << msaaSamples);
                // update renderer setting as needed
            }

            static float resolutionScale = 1.0f;
            if (ImGui::SliderFloat("Resolution Scale", &resolutionScale, 0.5f, 2.0f, "%.2fx")) {
                int newWidth = static_cast<int>(winSize.x * resolutionScale);
                int newHeight = static_cast<int>(winSize.y * resolutionScale);

                glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

                glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newWidth, newHeight);
            }

            static int shadowQuality = 1;
            const char* shadowOptions[] = { "Off", "Low", "Medium", "High" };
            if (ImGui::Combo("Shadow Quality", &shadowQuality, shadowOptions, IM_ARRAYSIZE(shadowOptions))) {
                // update shadows, not implemented yet
            }

            static bool enableBloom = true;
            if (ImGui::Checkbox("Bloom", &enableBloom)) {
                // not implemented yet
            }

            static bool enableSSAO = false;
            if (ImGui::Checkbox("Ambient Occlusion (SSAO)", &enableSSAO)) {
                // not implemented yet
            }

            ImGui::EndPopup();
        }

        if (isViewportHovered && !ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            if (ImGui::IsKeyPressed(ImGuiKey_W)) operation = ManipulationOperation::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_E)) operation = ManipulationOperation::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) operation = ManipulationOperation::SCALE;
            if (ImGui::IsKeyPressed(ImGuiKey_Q)) operation = ManipulationOperation::IDLE;
        }

        // --- GIZMO MATRIX HANDLING ---
		for (auto& selectedObject : Application->input->GetSelectedGameObjects())
		{
			if (operation != ManipulationOperation::IDLE && selectedObject != nullptr) {
				glm::mat4 displayMatrix = selectedObject->GetTransform()->GetMatrix();
				bool hasParent = selectedObject->GetParent() != nullptr;

				float matrixForManipulation[16];
				memcpy(matrixForManipulation, glm::value_ptr(displayMatrix), sizeof(float) * 16);

				float snap[3] = { Application->gui->UIinspectorPanel->snapValue,
								  Application->gui->UIinspectorPanel->snapValue,
								  Application->gui->UIinspectorPanel->snapValue };

				ImGuizmo::OPERATION guizmoOperation;
				switch (operation)
				{
				case ManipulationOperation::TRANSLATE: guizmoOperation = ImGuizmo::TRANSLATE; break;
				case ManipulationOperation::ROTATE:    guizmoOperation = ImGuizmo::ROTATE;    break;
				case ManipulationOperation::SCALE:     guizmoOperation = ImGuizmo::SCALE;     break;
				default: guizmoOperation = ImGuizmo::TRANSLATE; break;
				}

				ImGuizmo::MODE guizmoMode = (transformSpace == TransformSpace::WORLD) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

				ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
					guizmoOperation, guizmoMode, matrixForManipulation, NULL,
					(Application->gui->UIinspectorPanel->snap ? snap : nullptr));

				if (ImGuizmo::IsUsing()) {
					glm::mat4 manipulatedMatrix = glm::make_mat4(matrixForManipulation);

					if (hasParent) {
						glm::mat4 parentWorldMatrix = selectedObject->GetParent()->GetTransform()->GetMatrix();
						manipulatedMatrix = glm::inverse(parentWorldMatrix) * manipulatedMatrix;
					}

					selectedObject->GetTransform()->SetMatrix(manipulatedMatrix);
				}
			}
		}
        // --- END GIZMO MATRIX HANDLING ---

        isFoucused = ImGui::IsWindowHovered();
    }
    ImGui::End();

    ImGui::PopStyleVar();

    return true;
}

bool UISceneWindow::IsMouseOverWindow() const
{
    ImVec2 mousePos = ImGui::GetMousePos();

    float minX = winPos.x;
    float minY = winPos.y;
    float maxX = winPos.x + winSize.x;
    float maxY = winPos.y + winSize.y;

    bool isOverWindow = (mousePos.x >= minX && mousePos.x <= maxX &&
        mousePos.y >= minY && mousePos.y <= maxY);

    return isOverWindow;
}
