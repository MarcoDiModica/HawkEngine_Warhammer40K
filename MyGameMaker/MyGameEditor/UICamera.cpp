#include "UICamera.h"
#include "MyGUI.h"
#include "Input.h"
#include "MyWindow.h"

#include "../MyGameEngine/CameraComponent.h"

#include "imgui.h"

UICamera::UICamera(UIType type, std::string name) : UIElement(type, name)
{
}

UICamera::~UICamera()
{
	glDeleteFramebuffers(1, &Application->gui->fboCamera);
	glDeleteRenderbuffers(1, &Application->gui->rboCamera);
	glDeleteTextures(1, &Application->gui->fboTextureCamera);
}

void UICamera::Init()
{	
    glGenFramebuffers(1, &Application->gui->fboCamera);
    glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboCamera);

    glGenTextures(1, &Application->gui->fboTextureCamera);
    glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureCamera);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTextureCamera, 0);

    glGenRenderbuffers(1, &Application->gui->rboCamera);
    glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboCamera);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rboCamera);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Error: Framebuffer no está completo." << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UICamera::UpdateFramebuffer()
{
	int newWidth = Application->window->width();
	int newHeight = Application->window->height();

	// Actualizar la textura del framebuffer
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureCamera);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	// Actualizar el renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboCamera);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newWidth, newHeight);
}

bool UICamera::Draw()
{
	// Update the framebuffer if the window size has changed
	if (winSize.x != Application->window->width() || winSize.y != Application->window->height())
	{
		UpdateFramebuffer();
	}
	
	if (ImGui::Begin("Camera", &enabled))
	{
		// Define the view and projection matrices
		if (Application->root->mainCamera == nullptr)
		{
			ImGui::Text("No camera found");
			ImGui::End();
			return true;
		}
		else
		{
			CameraComponent* camera = Application->root->mainCamera->GetComponent<CameraComponent>();
			glm::mat4 viewMatrix = camera->view();
			glm::mat4 projectionMatrix = camera->projection();

			// Get the current window's position and size
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();

			winPos = vec2(windowPos.x, windowPos.y);
			winSize = vec2(windowSize.x, windowSize.y);

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
			ImGui::Image((ImTextureID)(uintptr_t)Application->gui->fboTextureCamera, imageSize, ImVec2(0, 1), ImVec2(1, 0));

			// Convert matrices to float arrays
			float view[16];
			float projection[16];
			memcpy(view, &viewMatrix, sizeof(float) * 16);
			memcpy(projection, &projectionMatrix, sizeof(float) * 16);

			// Draw the camera's view and projection matrices
			ImGui::Text("View matrix");
			ImGui::Text("X: %f, %f, %f, %f", viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], viewMatrix[3][0]);
			ImGui::Text("Y: %f, %f, %f, %f", viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], viewMatrix[3][1]);

			ImGui::Text("Projection matrix");
			ImGui::Text("X: %f, %f, %f, %f", projectionMatrix[0][0], projectionMatrix[1][0], projectionMatrix[2][0], projectionMatrix[3][0]);
			ImGui::Text("Y: %f, %f, %f, %f", projectionMatrix[0][1], projectionMatrix[1][1], projectionMatrix[2][1], projectionMatrix[3][1]);
		}
		

		ImGui::End();
	}

	return true;

}