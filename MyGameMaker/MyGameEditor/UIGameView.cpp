#include <imgui.h>

#include "UIGameView.h"
#include "MyGUI.h"
#include "MyWindow.h"
#include "App.h"
#include "MyGameEngine/CameraComponent.h"
#include <unordered_map>

class TextureResourceManager {
private:
	struct TextureResource {
		GLuint textureId = 0;
		GLuint framebufferId = 0;
		GLuint renderbufferId = 0;
		int width = 0;
		int height = 0;
		bool dirty = false;
		bool initialized = false;
	};

	std::unordered_map<std::string, TextureResource> resources;

public:
	void InitResource(const std::string& name, int width, int height) {
		if (resources.find(name) != resources.end() && resources[name].initialized) {
			return;
		}

		auto& resource = resources[name];
		resource.width = width;
		resource.height = height;
		resource.dirty = false;

		if (resource.framebufferId == 0) {
			glGenFramebuffers(1, &resource.framebufferId);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, resource.framebufferId);

		if (resource.textureId == 0) {
			glGenTextures(1, &resource.textureId);
		}

		glBindTexture(GL_TEXTURE_2D, resource.textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resource.textureId, 0);

		if (resource.renderbufferId == 0) {
			glGenRenderbuffers(1, &resource.renderbufferId);
		}

		glBindRenderbuffer(GL_RENDERBUFFER, resource.renderbufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, resource.renderbufferId);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG(LogType::LOG_ERROR, "Framebuffer '%s' is not complete!", name.c_str());
		}

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		resource.initialized = true;
	}

	bool UpdateSize(const std::string& name, int width, int height) {
		if (resources.find(name) == resources.end()) {
			InitResource(name, width, height);
			return true;
		}

		auto& resource = resources[name];

		if (abs(resource.width - width) < 5 && abs(resource.height - height) < 5) {
			return false;
		}

		resource.width = width;
		resource.height = height;
		resource.dirty = true;

		GLint lastFBO = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFBO);

		glBindFramebuffer(GL_FRAMEBUFFER, resource.framebufferId);
		glBindTexture(GL_TEXTURE_2D, resource.textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindRenderbuffer(GL_RENDERBUFFER, resource.renderbufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);

		return true;
	}

	void GenerateMipmaps(const std::string& name) {
		if (resources.find(name) == resources.end()) {
			return;
		}

		glBindTexture(GL_TEXTURE_2D, resources[name].textureId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint GetFramebuffer(const std::string& name) {
		if (resources.find(name) == resources.end()) {
			return 0;
		}

		return resources[name].framebufferId;
	}

	GLuint GetTexture(const std::string& name) {
		if (resources.find(name) == resources.end()) {
			return 0;
		}

		return resources[name].textureId;
	}

	bool IsDirty(const std::string& name) {
		if (resources.find(name) == resources.end()) {
			return false;
		}

		return resources[name].dirty;
	}

	void MarkClean(const std::string& name) {
		if (resources.find(name) == resources.end()) {
			return;
		}

		resources[name].dirty = false;
	}

	void Cleanup() {
		for (auto& pair : resources) {
			auto& resource = pair.second;

			if (resource.renderbufferId != 0) {
				glDeleteRenderbuffers(1, &resource.renderbufferId);
				resource.renderbufferId = 0;
			}

			if (resource.textureId != 0) {
				glDeleteTextures(1, &resource.textureId);
				resource.textureId = 0;
			}

			if (resource.framebufferId != 0) {
				glDeleteFramebuffers(1, &resource.framebufferId);
				resource.framebufferId = 0;
			}

			resource.initialized = false;
		}

		resources.clear();
	}
};

// Singleton instance
static TextureResourceManager& GetTextureManager() {
	static TextureResourceManager instance;
	return instance;
}

UIGameView::UIGameView(UIType type, std::string name) : UIElement(type, name)
{
}

UIGameView::~UIGameView()
{
	GetTextureManager().Cleanup();
}

void UIGameView::Init()
{
	GetTextureManager().InitResource("gameview", 1920, 1080);

	// Store references in GUI
	Application->gui->fboGame = GetTextureManager().GetFramebuffer("gameview");
	Application->gui->fboTextureGame = GetTextureManager().GetTexture("gameview");
	Application->gui->rboGame = 0; 
}

void UIGameView::UpdateFramebuffer()
{
	if (GetTextureManager().UpdateSize("gameview", 1920, 1080)) {
		Application->gui->fboGame = GetTextureManager().GetFramebuffer("gameview");
		Application->gui->fboTextureGame = GetTextureManager().GetTexture("gameview");
	}
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

		static float lastWidth = 0;
		static float lastHeight = 0;

		if (abs(width - lastWidth) > 20 || abs(height - lastHeight) > 20) {
			GetTextureManager().UpdateSize("gameview", static_cast<int>(width), static_cast<int>(height));
			lastWidth = width;
			lastHeight = height;
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

		if (GetTextureManager().IsDirty("gameview")) {
			GetTextureManager().GenerateMipmaps("gameview");
			GetTextureManager().MarkClean("gameview");
		}

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