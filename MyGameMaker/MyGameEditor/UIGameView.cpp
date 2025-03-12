#include <imgui.h>

#include "UIGameView.h"
#include "MyGUI.h"
#include "MyWindow.h"
#include "App.h"
#include "MyGameEngine/CameraComponent.h"

UIGameView::UIGameView(UIType type, std::string name) : UIElement(type, name)
{
}

UIGameView::~UIGameView()
{
	if (Application->gui->fboGame != 0) {
		glDeleteFramebuffers(1, &Application->gui->fboGame);
		glDeleteRenderbuffers(1, &Application->gui->rboGame);
		glDeleteTextures(1, &Application->gui->fboTextureGame);
	}
}

void UIGameView::Init()
{
	GLint lastFBO = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFBO);

	glGenFramebuffers(1, &Application->gui->fboGame);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);

	glGenTextures(1, &Application->gui->fboTextureGame);
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTextureGame, 0);

	glGenRenderbuffers(1, &Application->gui->rboGame);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboGame);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rboGame);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(LogType::LOG_ERROR, "Game View Framebuffer is not complete!");
	}

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);
}

// Modificaci�n para UIGameView.cpp

void UIGameView::UpdateFramebuffer()
{
	GLint lastFBO = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFBO);

	int newWidth = 1920;  
	int newHeight = 1080; 

	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);

	if (Application->gui->fboTextureGame != 0) {
		glDeleteTextures(1, &Application->gui->fboTextureGame);
	}

	glGenTextures(1, &Application->gui->fboTextureGame);
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTextureGame, 0);

	if (Application->gui->rboGame != 0) {
		glDeleteRenderbuffers(1, &Application->gui->rboGame);
	}

	glGenRenderbuffers(1, &Application->gui->rboGame);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboGame);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newWidth, newHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rboGame);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(LogType::LOG_ERROR, "Game View Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);
}

bool UIGameView::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | 
                            ImGuiWindowFlags_NoScrollWithMouse | 
                            ImGuiWindowFlags_NoNavInputs |
                            ImGuiWindowFlags_NoTitleBar;
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::Begin("Game View", &enabled, ImGuiWindowFlags_NoScrollbar))
	{
		if (Application->root->mainCamera == nullptr)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "No game camera found!");
			ImGui::TextWrapped("Create a GameObject with a CameraComponent and set it as main camera.");
			ImGui::End();
			return true;
		}

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		winPos = vec2(windowPos.x, windowPos.y);
		winSize = vec2(windowSize.x, windowSize.y);

        //debug viewport size and position
        ImGui::Text("Viewport Size: %f, %f", width, height);
        ImGui::Text("Viewport Pos: %f, %f", GetViewportPos().x, GetViewportPos().y);

		float availableAspectRatio = availableSize.x / availableSize.y;

        const float targetAspectRatio = 16.0f / 9.0f;

        width = availableSize.x;
        height = width / targetAspectRatio;
		
		float offsetX = 0, offsetY = 0;

		if (availableAspectRatio > targetAspectRatio) {
			height = availableSize.y;
			width = height * targetAspectRatio;

			offsetX = (availableSize.x - width) * 0.5f;
		}
		else {
			width = availableSize.x;
			height = width / targetAspectRatio;

			offsetY = (availableSize.y - height) * 0.5f;
		}

		ImVec2 viewportMin = ImGui::GetItemRectMin();
		viewportPos = vec2(viewportMin.x, viewportMin.y);
		viewportSize = vec2(width, height);

        ImGui::End();
    }

    return true;
}

vec2 UIGameView::GetViewportSize()
{
    return viewportSize;
}

vec2 UIGameView::GetViewportPos()
{
	return viewportPos;
		if (offsetX > 0) {
			ImGui::Indent(offsetX);
		}

		if (offsetY > 0) {
			ImGui::Dummy(ImVec2(0, offsetY));
		}

		ImGui::Image(
			(ImTextureID)(uintptr_t)Application->gui->fboTextureGame,
			ImVec2(width, height),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		winPos = vec2(windowPos.x, windowPos.y);
		winSize = vec2(windowSize.x, windowSize.y);

		ImGui::End();
	}

	ImGui::PopStyleVar();

	return true;
}