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

	if (msaaFbo != 0) {
		glDeleteFramebuffers(1, &msaaFbo);
		glDeleteRenderbuffers(1, &msaaColorRbo);
		glDeleteRenderbuffers(1, &msaaDepthRbo);
	}
}

void UIGameView::Init()
{
	GLint lastFBO = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFBO);

	GLint maxSamples = 0;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
	msaaSamples = std::min(4, maxSamples);
	useMSAA = msaaSamples > 0;

	glGenFramebuffers(1, &Application->gui->fboGame);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);

	glGenTextures(1, &Application->gui->fboTextureGame);
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (GLEW_EXT_texture_filter_anisotropic) {
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTextureGame, 0);

	glGenRenderbuffers(1, &Application->gui->rboGame);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboGame);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rboGame);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(LogType::LOG_ERROR, "Game View Framebuffer is not complete!");
	}

	if (useMSAA) {
		glGenFramebuffers(1, &msaaFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);

		glGenRenderbuffers(1, &msaaColorRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, msaaColorRbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA8,
			Application->window->width(), Application->window->height());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorRbo);

		glGenRenderbuffers(1, &msaaDepthRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, msaaDepthRbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8,
			Application->window->width(), Application->window->height());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaDepthRbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG(LogType::LOG_ERROR, "Game View MSAA Framebuffer is not complete, disabling MSAA!");

			glDeleteRenderbuffers(1, &msaaColorRbo);
			glDeleteRenderbuffers(1, &msaaDepthRbo);
			glDeleteFramebuffers(1, &msaaFbo);

			msaaColorRbo = 0;
			msaaDepthRbo = 0;
			msaaFbo = 0;
			useMSAA = false;
		}
	}

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);
}

void UIGameView::UpdateFramebuffer()
{
	GLint lastFBO = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFBO);

	int newWidth = Application->window->width();
	int newHeight = Application->window->height();

	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);

	if (Application->gui->fboTextureGame != 0) {
		glDeleteTextures(1, &Application->gui->fboTextureGame);
	}

	glGenTextures(1, &Application->gui->fboTextureGame);
	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (GLEW_EXT_texture_filter_anisotropic) {
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
	}

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

	if (useMSAA) {
		if (msaaFbo != 0) {
			glDeleteRenderbuffers(1, &msaaColorRbo);
			glDeleteRenderbuffers(1, &msaaDepthRbo);
			glDeleteFramebuffers(1, &msaaFbo);
		}

		glGenFramebuffers(1, &msaaFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);

		glGenRenderbuffers(1, &msaaColorRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, msaaColorRbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA8, newWidth, newHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorRbo);

		glGenRenderbuffers(1, &msaaDepthRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, msaaDepthRbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, newWidth, newHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaDepthRbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG(LogType::LOG_ERROR, "Game View MSAA Framebuffer update failed, disabling MSAA!");

			glDeleteRenderbuffers(1, &msaaColorRbo);
			glDeleteRenderbuffers(1, &msaaDepthRbo);
			glDeleteFramebuffers(1, &msaaFbo);

			msaaColorRbo = 0;
			msaaDepthRbo = 0;
			msaaFbo = 0;
			useMSAA = false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);
}

void UIGameView::BeginRender()
{
	if (!useMSAA) {
		glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);
	}

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void UIGameView::EndRender()
{
	if (useMSAA) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Application->gui->fboGame);
		glBlitFramebuffer(0, 0, Application->window->width(), Application->window->height(),
			0, 0, Application->window->width(), Application->window->height(),
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool UIGameView::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoNavInputs |
		ImGuiWindowFlags_NoTitleBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::Begin("Game View", &enabled, flags))
	{
		if (Application->root->mainCamera == nullptr)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "No game camera found!");
			ImGui::TextWrapped("Create a GameObject with a CameraComponent and set it as main camera.");
			ImGui::End();
			ImGui::PopStyleVar();
			return true;
		}

		ImVec2 availableSize = ImGui::GetContentRegionAvail();
		const float targetAspectRatio = 16.0f / 9.0f;
		float availableAspectRatio = availableSize.x / availableSize.y;
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

		ImVec2 viewportMin = ImGui::GetItemRectMin();
		viewportPos = vec2(viewportMin.x, viewportMin.y);
		viewportSize = vec2(width, height);

		ImGui::End();
	}

	ImGui::PopStyleVar();

	return true;
}

vec2 UIGameView::GetViewportSize()
{
	return viewportSize;
}

vec2 UIGameView::GetViewportPos()
{
	return viewportPos;
}