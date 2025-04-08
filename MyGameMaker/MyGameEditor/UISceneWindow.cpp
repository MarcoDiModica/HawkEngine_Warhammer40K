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
#include <functional>

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
}

UISceneWindow::~UISceneWindow()
{
}

void UISceneWindow::Init()
{
	GLint maxSamples = 0;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

	if (maxSamples <= 0) {
		msaaSamples = 0;
	}
	else {
		msaaSamples = std::min(msaaSamples, maxSamples);
	}

	if (msaaSamples > 0) {
		glGenFramebuffers(1, &Application->gui->multisampleFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->multisampleFBO);

		glGenRenderbuffers(1, &Application->gui->multisampleColorBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->multisampleColorBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA16F,
			Application->window->width(), Application->window->height());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_RENDERBUFFER, Application->gui->multisampleColorBuffer);

		glGenRenderbuffers(1, &Application->gui->multisampleDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->multisampleDepthBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8,
			Application->window->width(), Application->window->height());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, Application->gui->multisampleDepthBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Error: Multisample framebuffer is not complete! Falling back to non-MSAA." << std::endl;
			glDeleteRenderbuffers(1, &Application->gui->multisampleColorBuffer);
			glDeleteRenderbuffers(1, &Application->gui->multisampleDepthBuffer);
			glDeleteFramebuffers(1, &Application->gui->multisampleFBO);
			msaaSamples = 0;
		}
	}

	glGenFramebuffers(1, &Application->gui->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);

	glGenTextures(1, &Application->gui->fboTexture);
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Application->window->width(),
		Application->window->height(), 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (GLEW_EXT_texture_filter_anisotropic)
	{
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		Application->gui->fboTexture, 0);

	glGenRenderbuffers(1, &Application->gui->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		Application->window->width(), Application->window->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER, Application->gui->rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error: Framebuffer is not complete!" << std::endl;

	lastWidth = Application->window->width();
	lastHeight = Application->window->height();

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

bool UISceneWindow::CheckRayAABBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const BoundingBox& bBox, glm::vec3& collisionPoint) 
{
	glm::vec3 dirInv = glm::vec3(
		rayDir.x != 0.0f ? 1.0f / rayDir.x : std::numeric_limits<float>::infinity(),
		rayDir.y != 0.0f ? 1.0f / rayDir.y : std::numeric_limits<float>::infinity(),
		rayDir.z != 0.0f ? 1.0f / rayDir.z : std::numeric_limits<float>::infinity()
	);

	float t1 = (bBox.min.x - rayOrigin.x) * dirInv.x;
	float t2 = (bBox.max.x - rayOrigin.x) * dirInv.x;
	float t3 = (bBox.min.y - rayOrigin.y) * dirInv.y;
	float t4 = (bBox.max.y - rayOrigin.y) * dirInv.y;
	float t5 = (bBox.min.z - rayOrigin.z) * dirInv.z;
	float t6 = (bBox.max.z - rayOrigin.z) * dirInv.z;

	float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
	float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

	if (tmax < 0) {
		return false;
	}

	if (tmin > tmax) {
		return false;
	}

	float t = tmin < 0 ? tmax : tmin;

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

		if (abs((int)winSize.x - (int)contentRegionSize.x) > 20 ||
			abs((int)winSize.y - (int)contentRegionSize.y) > 20)
		{
			winSize = vec2(contentRegionSize.x, contentRegionSize.y);
			needsFramebufferUpdate = true;
		}

		if (needsFramebufferUpdate && winSize.x > 1 && winSize.y > 1)
		{
			if (msaaSamples > 0) {
				glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->multisampleColorBuffer);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA16F, (int)winSize.x, (int)winSize.y);

				glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->multisampleDepthBuffer);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, (int)winSize.x, (int)winSize.y);
			}

			glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)winSize.x, (int)winSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);

			glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)winSize.x, (int)winSize.y);

			Application->camera->UpdateCameraView(winSize.x, winSize.y, winSize.x, winSize.y);
			Application->gui->camSize = winSize;

			lastWidth = (int)winSize.x;
			lastHeight = (int)winSize.y;
			needsFramebufferUpdate = false;
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
        static TransformSpace transformSpace = TransformSpace::WORLD;

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

        static ManipulationOperation operation = ManipulationOperation::TRANSLATE;

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

				/*	if (hasParent) {
						glm::mat4 parentWorldMatrix = selectedObject->GetParent()->GetTransform()->GetMatrix();
						manipulatedMatrix = glm::inverse(parentWorldMatrix) * manipulatedMatrix;
				}*/

					selectedObject->GetTransform()->SetMatrix(manipulatedMatrix);
				}
			}
		}
        // --- END GIZMO MATRIX HANDLING ---

        isFoucused = ImGui::IsWindowHovered();

		bool isShiftHeld = ImGui::GetIO().KeyShift;
		bool isCtrlHeld = ImGui::GetIO().KeyCtrl;

		if (isViewportHovered && !ImGuizmo::IsUsing() && !ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			bool isOverUIButtons = (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) || ImGui::IsAnyItemHovered());

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver() &&
				!isShiftHeld && !isCtrlHeld && !isOverUIButtons) {
				isMarqueeSelecting = true;
				marqueeStart = ImGui::GetMousePos();
			}

			if (isMarqueeSelecting) {
				marqueeEnd = ImGui::GetMousePos();

				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddRect(
					marqueeStart,
					marqueeEnd,
					IM_COL32(0, 255, 255, 255),
					0.0f,                   
					NULL,
					2.0f            
				);

				drawList->AddRectFilled(
					marqueeStart,
					marqueeEnd,
					IM_COL32(0, 255, 255, 64) 
				);
			}

			if (isMarqueeSelecting && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				float dragDistance = glm::distance(
					glm::vec2(marqueeStart.x, marqueeStart.y),
					glm::vec2(marqueeEnd.x, marqueeEnd.y)
				);

				if (dragDistance > 5.0f) { 
					if (!isShiftHeld && !isCtrlHeld) {
						Application->input->ClearSelection();
					}

					auto activeScene = Application->root->GetActiveScene();
					if (activeScene) {
						std::vector<GameObject*> allObjects;
						for (const auto& objPtr : activeScene->children()) {
							if (objPtr && objPtr->IsActive()) {
								GameObject* object = objPtr.get();
								allObjects.push_back(object);

								std::function<void(GameObject*, std::vector<GameObject*>&)> collectChildren;
								collectChildren = [&collectChildren](GameObject* parent, std::vector<GameObject*>& list) {
									for (const auto& childPtr : parent->GetChildren()) {
										if (childPtr && childPtr->IsActive()) {
											GameObject* child = childPtr.get();
											list.push_back(child);
											collectChildren(child, list);
										}
									}
									};

								collectChildren(object, allObjects);
							}
						}

						for (GameObject* obj : allObjects) {
							if (IsGameObjectInMarquee(obj, marqueeStart, marqueeEnd)) {
								if (isCtrlHeld && Application->input->IsGameObjectSelected(obj)) {
									Application->input->RemoveFromSelection(obj);
								}
								else {
									Application->input->AddToSelection(obj);
								}
							}
						}
					}
				}

				isMarqueeSelecting = false;
			}
		}
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

bool UISceneWindow::IsGameObjectInMarquee(GameObject* gameObject, const ImVec2& start, const ImVec2& end) {
	if (!gameObject->HasComponent<MeshRenderer>()) return false;

	glm::mat4 viewMatrix = Application->camera->view();
	glm::mat4 projectionMatrix = Application->camera->projection();
	glm::mat4 modelMatrix = gameObject->GetTransform()->GetMatrix();
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

	BoundingBox bbox = gameObject->GetComponent<MeshRenderer>()->GetMesh()->boundingBox();
	std::vector<glm::vec3> corners = {
		glm::vec3(bbox.min.x, bbox.min.y, bbox.min.z),
		glm::vec3(bbox.max.x, bbox.min.y, bbox.min.z),
		glm::vec3(bbox.min.x, bbox.max.y, bbox.min.z),
		glm::vec3(bbox.max.x, bbox.max.y, bbox.min.z),
		glm::vec3(bbox.min.x, bbox.min.y, bbox.max.z),
		glm::vec3(bbox.max.x, bbox.min.y, bbox.max.z),
		glm::vec3(bbox.min.x, bbox.max.y, bbox.max.z),
		glm::vec3(bbox.max.x, bbox.max.y, bbox.max.z)
	};

	bool anyInside = false;
	bool allOutside = true;

	for (const auto& corner : corners) {
		glm::vec4 clipSpace = mvp * glm::vec4(corner, 1.0f);

		if (clipSpace.w != 0.0f) {
			glm::vec3 ndcPos = glm::vec3(clipSpace) / clipSpace.w;

			float screenX = (ndcPos.x * 0.5f + 0.5f) * winSize.x + winPos.x;
			float screenY = (ndcPos.y * -0.5f + 0.5f) * winSize.y + winPos.y;

			bool isInsideMarquee =
				screenX >= fmin(start.x, end.x) && screenX <= fmax(start.x, end.x) &&
				screenY >= fmin(start.y, end.y) && screenY <= fmax(start.y, end.y);

			if (isInsideMarquee) {
				anyInside = true;
			}
			else {
				allOutside = false;
			}
		}
	}

	return anyInside;
}