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


UISceneWindow::UISceneWindow(UIType type, std::string name) : UIElement(type, name)
{
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

bool UISceneWindow::Draw()
{
	ImGui::SetNextWindowPos(ImVec2(110, 30), ImGuiCond_Once); // Set the desired initial position
	ImGui::SetNextWindowSize(ImVec2(865, 485), ImGuiCond_Once);


	// Begin the GameWindow
	ImGui::Begin("Scene", nullptr, windowFlags);
	{
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

		// Because I use the texture from OpenGL, I need to invert the V from the UV.
		ImGui::Image((ImTextureID)(uintptr_t)Application->gui->fboTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

		glm::mat4 viewMatrix = Application->camera->view();
		glm::mat4 projectionMatrix = Application->camera->projection();

		float view[16];
		float projection[16];
		memcpy(view, &viewMatrix, sizeof(float) * 16);
		memcpy(projection, &projectionMatrix, sizeof(float) * 16);

		auto selectedGameObjects = Application->input->GetSelectedGameObjects();

		if (!selectedGameObjects.empty()) {
			glm::vec3 averagePosition(0.0f);

			for (GameObject* selectedObject : selectedGameObjects) {
				if (selectedObject != nullptr) {
					averagePosition += selectedObject->GetTransform()->GetPosition();
				}
			}
			averagePosition /= static_cast<float>(selectedGameObjects.size());

			glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), averagePosition);
			float gizmoMatrixArray[16];
			memcpy(gizmoMatrixArray, glm::value_ptr(gizmoMatrix), sizeof(float) * 16);

			float snap[3] = {
				1.0f * Application->gui->UIinspectorPanel->snapValue,
				1.0f * Application->gui->UIinspectorPanel->snapValue,
				1.0f * Application->gui->UIinspectorPanel->snapValue
			};

			if (Application->gui->UIinspectorPanel->snap) {
				ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE,
					ImGuizmo::LOCAL, gizmoMatrixArray, nullptr, snap);
			}
			else {
				ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE,
					ImGuizmo::LOCAL, gizmoMatrixArray, nullptr);
			}

			glm::vec3 newPosition, newRotation, newScale;
			ImGuizmo::DecomposeMatrixToComponents(gizmoMatrixArray, glm::value_ptr(newPosition),
				glm::value_ptr(newRotation), glm::value_ptr(newScale));

			// Calcular la diferencia de escala del gizmo
			glm::vec3 gizmoScaleDelta = newScale / gizmoOriginalScale;

			for (GameObject* selectedObject : selectedGameObjects) {
				if (selectedObject != nullptr) {
					// Calcular la posición relativa y la nueva posición
					glm::vec3 objectPosition = selectedObject->GetTransform()->GetPosition();
					glm::vec3 relativePosition = objectPosition - averagePosition;
					glm::vec3 finalPosition = newPosition + relativePosition;

					// Rotación
					glm::quat objectRotationQuat = selectedObject->GetTransform()->GetRotation();

					// Convertir la rotación del gizmo a quaternion
					glm::quat gizmoRotationQuat = glm::quat(glm::radians(newRotation));

					// Combinar la rotación del objeto con la rotación del gizmo
					glm::quat finalRotationQuat = gizmoRotationQuat * objectRotationQuat;

					// Convertir la rotación final a Euler angles (glm::vec3)
					glm::vec3 finalRotationEuler = glm::degrees(glm::eulerAngles(finalRotationQuat));

					// Escala
					glm::vec3 objectScale = selectedObject->GetTransform()->GetScale();
					glm::vec3 finalScale = objectScale * gizmoScaleDelta;

					// Actualizar las transformaciones en el objeto seleccionado
					selectedObject->GetTransform()->SetPosition(finalPosition);

					// Usar SetRotation con glm::vec3
					selectedObject->GetTransform()->SetRotation(finalRotationEuler);

					selectedObject->GetTransform()->SetScale(finalScale);
				}
			}

			// Actualizar la escala original del gizmo
			gizmoOriginalScale = newScale;
		}

		ImGui::End();
		return true;
	}
}

//peta cuando

//bool UISceneWindow::Draw()
//{
//	ImGui::SetNextWindowPos(ImVec2(110, 30), ImGuiCond_Once); // Set the desired initial position
//	ImGui::SetNextWindowSize(ImVec2(865, 485), ImGuiCond_Once);
//
//
//	// Begin the GameWindow
//	ImGui::Begin("Scene", nullptr, windowFlags);
//	{
//		// Get the current window's position and size
//		ImVec2 windowPos = ImGui::GetWindowPos();
//		ImVec2 windowSize = ImGui::GetWindowSize();
//
//		winPos = vec2(windowPos.x, windowPos.y);
//		winSize = vec2(windowSize.x, windowSize.y);
//		// Define the title bar rectangle
//		ImRect titleBarRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + ImGui::GetFrameHeight()));
//
//		// Get the current mouse position
//		ImVec2 mousePos = ImGui::GetMousePos();
//
//		// Check if the mouse is over the title bar of the GameWindow
//		bool isMouseOverTitleBar = (mousePos.x >= titleBarRect.Min.x && mousePos.x <= titleBarRect.Max.x &&
//			mousePos.y >= titleBarRect.Min.y && mousePos.y <= titleBarRect.Max.y);
//
//		// If the mouse is over the title bar, allow the window to be moved
//		if (isMouseOverTitleBar) {
//			windowFlags &= ~ImGuiWindowFlags_NoMove;
//		}
//		else {
//			windowFlags |= ImGuiWindowFlags_NoMove;
//		}
//
//		ImVec2 availableSize = ImGui::GetContentRegionAvail();
//
//		// Original image size (replace with your actual image size)
//		float originalWidth = 1280.0f; // Example width
//		float originalHeight = 720.0f; // Example height
//
//		// Calculate the aspect ratio of the original image
//		float aspectRatio = originalWidth / originalHeight;
//
//		// Calculate the new size for the image that fits within the GameWindow while maintaining the aspect ratio
//		ImVec2 imageSize = ImVec2(windowSize.x, windowSize.y);
//
//		// Calculate the offset needed to center the image
//		float offsetX = (availableSize.x - imageSize.x) / 2.0f;
//		float offsetY = (availableSize.y - imageSize.y) / 2.0f;
//
//		// Set the cursor position to the calculated offset
//		ImGui::SetCursorPos(ImVec2(offsetX, offsetY));
//
//		// Because I use the texture from OpenGL, I need to invert the V from the UV.
//		ImGui::Image((ImTextureID)(uintptr_t)Application->gui->fboTexture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
//
//		ImGuizmo::SetDrawlist();
//		ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
//
//		glm::mat4 viewMatrix = Application->camera->view();
//		glm::mat4 projectionMatrix = Application->camera->projection();
//
//		float view[16];
//		float projection[16];
//		memcpy(view, &viewMatrix, sizeof(float) * 16);
//		memcpy(projection, &projectionMatrix, sizeof(float) * 16);
//
//		// Itera sobre la lista de GameObjects seleccionados
//		for (GameObject* selectedObject : Application->input->GetSelectedGameObjects())
//		{
//			if (selectedObject != nullptr) {
//				glm::mat4 matrix = selectedObject->GetTransform()->GetMatrix();
//				float objectMatrix[16];
//				memcpy(objectMatrix, &matrix, sizeof(float) * 16);
//
//				float snap[3] = { 1.0f * Application->gui->UIinspectorPanel->snapValue,
//								  1.0f * Application->gui->UIinspectorPanel->snapValue,
//								  1.0f * Application->gui->UIinspectorPanel->snapValue };
//
//				if (Application->gui->UIinspectorPanel->snap) {
//					ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE,
//						ImGuizmo::LOCAL, objectMatrix, NULL, snap);
//				}
//				else {
//					ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE,
//						ImGuizmo::LOCAL, objectMatrix, NULL);
//				}
//
//				glm::vec3 position, rotation, scale;
//				ImGuizmo::DecomposeMatrixToComponents(objectMatrix, glm::value_ptr(position),
//					glm::value_ptr(rotation), glm::value_ptr(scale));
//				ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(position), glm::value_ptr(rotation),
//					glm::value_ptr(scale), objectMatrix);
//
//				glm::mat4 newMatrix = glm::make_mat4(objectMatrix);
//				selectedObject->GetTransform()->SetMatrix(newMatrix);
//			}
//		}
//	}
//	ImGui::End();
//	return true;
//}