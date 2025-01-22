#include "UISceneWindow.h"
#include "UIInspector.h"
#include "App.h"
#include "MyGUI.h"
#include <imgui.h>
#include <imgui_internal.h>	
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include "../MyGameEngine/MeshRendererComponent.h"

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
	// Generate and bind the FBO
	glGenFramebuffers(1, &Application->gui->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);

	// Create the texture to render to
	glGenTextures(1, &Application->gui->fboTexture);
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTexture, 0);

	// Create a renderbuffer object for depth and stencil attachments
	glGenRenderbuffers(1, &Application->gui->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rbo);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Error: Framebuffer is not complete!" << std::endl;

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiConfigFlags_DockingEnable;;

glm::vec3 UISceneWindow::ConvertMouseToWorldCoords(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y)
{
	int adjusted_mouse_x = mouse_x - window_x;
	int adjusted_mouse_y = mouse_y - window_y;

	if (adjusted_mouse_x < 0 || adjusted_mouse_x > screen_width || adjusted_mouse_y < 0 || adjusted_mouse_y > screen_height) {
		// Si el ratón está fuera de la ventana, no se detecta nada
	}

	float ndc_x = (2.0f * adjusted_mouse_x) / screen_width - 1.0f;
	float ndc_y = 1.0f - (2.0f * adjusted_mouse_y) / screen_height;

	glm::vec4 clip_coords = glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);
	glm::mat4 projection_matrix = Application->camera->projection();
	glm::vec4 view_coords = glm::inverse(projection_matrix) * clip_coords;
	view_coords = glm::vec4(view_coords.x, view_coords.y, -1.0f, 0.0f);

	glm::mat4 view_matrix = Application->camera->view();
	glm::vec4 world_coords = glm::inverse(view_matrix) * view_coords;

	return glm::vec3(world_coords.x + Application->camera->GetTransform().GetPosition().x, world_coords.y + Application->camera->GetTransform().GetPosition().y, world_coords.z + Application->camera->GetTransform().GetPosition().z);
}

glm::vec3 UISceneWindow::GetMousePickDir(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y)
{
	int adjusted_mouse_x = mouse_x - window_x;
	int adjusted_mouse_y = mouse_y - window_y;

	if (adjusted_mouse_x < 0 || adjusted_mouse_x > screen_width || adjusted_mouse_y < 0 || adjusted_mouse_y > screen_height) {
		// Si el ratón está fuera de la ventana, no se detecta nada
	}

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
	// Begin the GameWindow
	ImGui::Begin("Scene", nullptr, windowFlags);
	{
		bool isViewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

		// Get the current window's position and size
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		winPos = vec2(windowPos.x, windowPos.y);
		winSize = vec2(windowSize.x, windowSize.y);
		// Define the title bar rectangle
		ImRect titleBarRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + ImGui::GetFrameHeight()));

		// Get the current mouse position
		ImVec2 mousePos = ImGui::GetMousePos();

		// Check if the mouse is over the title bar of the GameWindow
		bool isMouseOverTitleBar = (mousePos.x >= titleBarRect.Min.x && mousePos.x <= titleBarRect.Max.x &&
			mousePos.y >= titleBarRect.Min.y && mousePos.y <= titleBarRect.Max.y);

		// If the mouse is over the title bar, allow the window to be moved
		if (isMouseOverTitleBar) {
			windowFlags &= ~ImGuiWindowFlags_NoMove;
		}
		else {
			windowFlags |= ImGuiWindowFlags_NoMove;
		}

		ImVec2 availableSize = ImGui::GetContentRegionAvail();

		// Original image size (replace with your actual image size)
		float originalWidth = 1280.0f; // Example width
		float originalHeight = 720.0f; // Example height

		// Calculate the aspect ratio of the original image
		float aspectRatio = originalWidth / originalHeight;

		// Calculate the new size for the image that fits within the GameWindow while maintaining the aspect ratio
		ImVec2 imageSize = ImVec2(windowSize.x, windowSize.y);

		// Calculate the offset needed to center the image
		float offsetX = (availableSize.x - imageSize.x) / 2.0f;
		float offsetY = (availableSize.y - imageSize.y) / 2.0f;

		// Set the cursor position to the calculated offset
		ImGui::SetCursorPos(ImVec2(offsetX, offsetY));

		// Position of the viewport in screen coordinates
		ImVec2 viewportPosition = ImGui::GetCursorScreenPos();

		// Because I use the texture from OpenGL, I need to invert the V from the UV.
		ImGui::Image((ImTextureID)(uintptr_t)Application->gui->fboTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGuizmo::SetDrawlist();

		// Set the ImGuizmo viewport
		ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

		// Define the view and projection matrices
		glm::mat4 viewMatrix = Application->camera->view();
		glm::mat4 projectionMatrix = Application->camera->projection();

		// Function to set button colors based on the active state
		auto setActiveButtonColor = [](bool isActive) {
			if (isActive) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.53f, 0.81f, 0.92f, 0.75f)); // Active state color
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.31f, 0.26f, 0.71f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 0.75f)); // Default
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
			}
			};

		// Position buttons at the top of the viewport
		ImGui::SetCursorScreenPos(ImVec2(viewportPosition.x + 180, viewportPosition.y + 60)); // Offset from top-left

		static TransformSpace transformSpace = TransformSpace::LOCAL;

		// World/Local button
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

		ImGui::SetCursorScreenPos(ImVec2(viewportPosition.x + 20, viewportPosition.y + 55)); // Offset from top-left

		// Manipulation types (idle, translate, rotate, scale)
		static ManipulationOperation operation = ManipulationOperation::IDLE;

		// Translate button
		setActiveButtonColor(operation == ManipulationOperation::TRANSLATE);
		if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Trans.id())), ImVec2(iconSize, iconSize))) {
			operation = (operation == ManipulationOperation::TRANSLATE) ? ManipulationOperation::IDLE : ManipulationOperation::TRANSLATE;
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine(0, iconSpacing);

		// Rotate button
		setActiveButtonColor(operation == ManipulationOperation::ROTATE);
		if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(m_Rot.id())), ImVec2(iconSize, iconSize))) {
			operation = (operation == ManipulationOperation::ROTATE) ? ManipulationOperation::IDLE : ManipulationOperation::ROTATE;
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine(0, iconSpacing);

		// Scale button
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

		// Convert matrices to float arrays
		float view[16];
		float projection[16];
		memcpy(view, &viewMatrix, sizeof(float) * 16);
		memcpy(projection, &projectionMatrix, sizeof(float) * 16);

		for (auto& selectedObject : Application->input->GetSelectedGameObjects())
		{
			if (operation != ManipulationOperation::IDLE && selectedObject != nullptr) {
				glm::mat4 matrix = selectedObject->GetTransform()->GetMatrix();

				ImGuizmo::OPERATION guizmoOperation;
				switch (operation)
				{
				case ManipulationOperation::TRANSLATE: guizmoOperation = ImGuizmo::TRANSLATE; break;
				case ManipulationOperation::ROTATE:    guizmoOperation = ImGuizmo::ROTATE;    break;
				case ManipulationOperation::SCALE:     guizmoOperation = ImGuizmo::SCALE;     break;
				default: guizmoOperation = ImGuizmo::TRANSLATE; break; // Fallback
				}

				ImGuizmo::MODE guizmoMode = (transformSpace == TransformSpace::LOCAL) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

				float objectMatrix[16];
				memcpy(objectMatrix, &matrix, sizeof(float) * 16);

				float snap[3] = { 1.0f * Application->gui->UIinspectorPanel->snapValue, 1.0f * Application->gui->UIinspectorPanel->snapValue, 1.0f * Application->gui->UIinspectorPanel->snapValue };

				ImGuizmo::Manipulate(view, projection, guizmoOperation, guizmoMode, objectMatrix, NULL, Application->gui->UIinspectorPanel->snap ? snap : nullptr);

				glm::vec3 position, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(objectMatrix, glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

				ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale), objectMatrix);

				glm::mat4 newMatrix = glm::make_mat4(objectMatrix);

				selectedObject->GetTransform()->SetMatrix(newMatrix);
			}
		}

		if (ImGui::IsWindowHovered())
		{
			isFoucused = true;
		}
		else
		{
			isFoucused = false;
		}
	}
	ImGui::End();

	return true;
}