#include <imgui.h>

#include "UIGameView.h"
#include "MyGUI.h"
#include "MyWindow.h"
#include "App.h"
#include "MyGameEngine/CameraComponent.h"
#include <unordered_map>

// Enhanced Texture resource manager with MSAA support
class GameViewResourceManager {
private:
	struct RenderResource {
		GLuint fbo = 0;                // Main framebuffer
		GLuint colorTexture = 0;       // Color texture attachment
		GLuint depthRbo = 0;           // Depth/stencil renderbuffer
		GLuint msaaFbo = 0;            // MSAA framebuffer
		GLuint msaaColorRbo = 0;       // MSAA color renderbuffer
		GLuint msaaDepthRbo = 0;       // MSAA depth/stencil renderbuffer
		int width = 1920;              // Fixed render width
		int height = 1080;             // Fixed render height
		int msaaSamples = 0;
		bool initialized = false;
		bool useMSAA = true;
	};

	RenderResource resource;

public:
	GameViewResourceManager() : resource{} {}

	void Initialize(int msaaSamples) {
		if (resource.initialized) {
			Cleanup();
		}

		// Always render at fixed 1920x1080 resolution
		resource.msaaSamples = msaaSamples;
		resource.useMSAA = msaaSamples > 0;

		// Get maximum supported MSAA samples
		if (resource.useMSAA) {
			GLint maxSamples = 0;
			glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
			resource.msaaSamples = std::min(resource.msaaSamples, maxSamples);
		}

		// Create main framebuffer
		glGenFramebuffers(1, &resource.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, resource.fbo);

		// Create color texture with mipmaps
		glGenTextures(1, &resource.colorTexture);
		glBindTexture(GL_TEXTURE_2D, resource.colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resource.width, resource.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// Setup texture filtering with mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Enable anisotropic filtering if available
		if (GLEW_EXT_texture_filter_anisotropic) {
			GLfloat maxAniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resource.colorTexture, 0);

		// Create depth/stencil renderbuffer
		glGenRenderbuffers(1, &resource.depthRbo);
		glBindRenderbuffer(GL_RENDERBUFFER, resource.depthRbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resource.width, resource.height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, resource.depthRbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG(LogType::LOG_ERROR, "GameView main framebuffer is not complete!");
		}

		// Create MSAA framebuffer if requested
		if (resource.useMSAA) {
			glGenFramebuffers(1, &resource.msaaFbo);
			glBindFramebuffer(GL_FRAMEBUFFER, resource.msaaFbo);

			// Create MSAA color renderbuffer
			glGenRenderbuffers(1, &resource.msaaColorRbo);
			glBindRenderbuffer(GL_RENDERBUFFER, resource.msaaColorRbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, resource.msaaSamples, GL_RGBA8, resource.width, resource.height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, resource.msaaColorRbo);

			// Create MSAA depth/stencil renderbuffer
			glGenRenderbuffers(1, &resource.msaaDepthRbo);
			glBindRenderbuffer(GL_RENDERBUFFER, resource.msaaDepthRbo);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, resource.msaaSamples, GL_DEPTH24_STENCIL8, resource.width, resource.height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, resource.msaaDepthRbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				LOG(LogType::LOG_ERROR, "GameView MSAA framebuffer is not complete, disabling MSAA!");

				// Cleanup MSAA resources
				glDeleteRenderbuffers(1, &resource.msaaColorRbo);
				glDeleteRenderbuffers(1, &resource.msaaDepthRbo);
				glDeleteFramebuffers(1, &resource.msaaFbo);

				resource.msaaColorRbo = 0;
				resource.msaaDepthRbo = 0;
				resource.msaaFbo = 0;
				resource.useMSAA = false;
			}
		}

		// Reset to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		resource.initialized = true;
	}

	void BeginRender() {
		if (!resource.initialized) {
			return;
		}

		// Bind the appropriate framebuffer
		if (resource.useMSAA) {
			glBindFramebuffer(GL_FRAMEBUFFER, resource.msaaFbo);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, resource.fbo);
		}

		// Clear with a dark color
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndRender() {
		if (!resource.initialized) {
			return;
		}

		// Resolve MSAA framebuffer if enabled
		if (resource.useMSAA) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, resource.msaaFbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resource.fbo);
			glBlitFramebuffer(0, 0, resource.width, resource.height,
				0, 0, resource.width, resource.height,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		// Generate mipmaps for the color texture
		glBindTexture(GL_TEXTURE_2D, resource.colorTexture);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Reset to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Cleanup() {
		// Cleanup MSAA resources
		if (resource.msaaColorRbo != 0) {
			glDeleteRenderbuffers(1, &resource.msaaColorRbo);
			resource.msaaColorRbo = 0;
		}

		if (resource.msaaDepthRbo != 0) {
			glDeleteRenderbuffers(1, &resource.msaaDepthRbo);
			resource.msaaDepthRbo = 0;
		}

		if (resource.msaaFbo != 0) {
			glDeleteFramebuffers(1, &resource.msaaFbo);
			resource.msaaFbo = 0;
		}

		// Cleanup main resources
		if (resource.depthRbo != 0) {
			glDeleteRenderbuffers(1, &resource.depthRbo);
			resource.depthRbo = 0;
		}

		if (resource.colorTexture != 0) {
			glDeleteTextures(1, &resource.colorTexture);
			resource.colorTexture = 0;
		}

		if (resource.fbo != 0) {
			glDeleteFramebuffers(1, &resource.fbo);
			resource.fbo = 0;
		}

		resource.initialized = false;
	}

	GLuint GetFramebuffer() const {
		return resource.fbo;
	}

	GLuint GetColorTexture() const {
		return resource.colorTexture;
	}

	int GetMSAASamples() const {
		return resource.msaaSamples;
	}

	bool IsMSAAEnabled() const {
		return resource.useMSAA;
	}

	int GetWidth() const {
		return resource.width;
	}

	int GetHeight() const {
		return resource.height;
	}
};

// Singleton instance
static GameViewResourceManager& GetGameViewResource() {
	static GameViewResourceManager instance;
	return instance;
}

UIGameView::UIGameView(UIType type, std::string name) : UIElement(type, name)
{
}

UIGameView::~UIGameView()
{
	// Cleanup resources
	GetGameViewResource().Cleanup();
}

void UIGameView::Init()
{
	// Initialize with 4x MSAA at fixed resolution
	GetGameViewResource().Initialize(4);

	// Store framebuffer references in the GUI
	Application->gui->fboGame = GetGameViewResource().GetFramebuffer();
	Application->gui->fboTextureGame = GetGameViewResource().GetColorTexture();
	Application->gui->rboGame = 0; // No longer directly accessed
}

void UIGameView::UpdateFramebuffer()
{
	// Nothing to do - we're using a fixed-size buffer now
}

bool UIGameView::Draw()
{
	// Use less flags to allow for a clean window display like in build mode
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoNavInputs;

	// Remove padding to allow full window usage
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	// Set window background to black for proper letterboxing
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

	if (ImGui::Begin("Game View", &enabled, flags))
	{
		if (Application->root->mainCamera == nullptr)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "No game camera found!");
			ImGui::TextWrapped("Create a GameObject with a CameraComponent and set it as main camera.");
			ImGui::End();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			return true;
		}

		// Get available window size
		ImVec2 availableSize = ImGui::GetContentRegionAvail();

		// Calculate scaling to maintain 16:9 aspect ratio while filling window
		float sourceWidth = static_cast<float>(GetGameViewResource().GetWidth());
		float sourceHeight = static_cast<float>(GetGameViewResource().GetHeight());
		float sourceAspect = sourceWidth / sourceHeight;

		float availWidth = availableSize.x;
		float availHeight = availableSize.y;
		float availAspect = availWidth / availHeight;

		float displayWidth, displayHeight;
		float offsetX = 0.0f, offsetY = 0.0f;

		// Scale and position to match build mode appearance (centered with letterboxing)
		if (availAspect > sourceAspect) {
			// Window is wider than source - fill height and center horizontally
			displayHeight = availHeight;
			displayWidth = displayHeight * sourceAspect;
			offsetX = (availWidth - displayWidth) * 0.5f;
		}
		else {
			// Window is taller than source - fill width and center vertically
			displayWidth = availWidth;
			displayHeight = displayWidth / sourceAspect;
			offsetY = (availHeight - displayHeight) * 0.5f;
		}

		// Set cursor position to apply offset (for letterboxing)
		if (offsetX > 0.0f || offsetY > 0.0f) {
			ImGui::SetCursorPos(ImVec2(offsetX, offsetY));
		}

		// Store dimensions for rendering and mouse input processing
		width = displayWidth;
		height = displayHeight;

		// Render the game texture at calculated size
		ImGui::Image(
			(ImTextureID)(uintptr_t)GetGameViewResource().GetColorTexture(),
			ImVec2(displayWidth, displayHeight),
			ImVec2(0, 1),  // UV coordinates for top-left
			ImVec2(1, 0)   // UV coordinates for bottom-right (flipped Y for OpenGL)
		);

		// Store viewport information for mouse input handling
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		winPos = vec2(windowPos.x, windowPos.y);
		winSize = vec2(windowSize.x, windowSize.y);

		ImVec2 viewportMin = ImGui::GetItemRectMin();
		viewportPos = vec2(viewportMin.x, viewportMin.y);
		viewportSize = vec2(displayWidth, displayHeight);

		ImGui::End();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

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