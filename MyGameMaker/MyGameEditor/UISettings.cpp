#include "UISettings.h"
#include "App.h"
#include "MyGUI.h"
#include "Input.h"
#include "HardwareSettings.h"
#include "assimp/version.h"
#include "IL/il.h"

#include "imgui.h"


UISettings::UISettings(UIType type, std::string name) : UIElement(type, name), fpsRecord(RECORD_SIZE), msRecord(RECORD_SIZE)
{
}


UISettings::~UISettings()
{
}

void UISettings::AddFpsMark(int fps)
{
	fpsRecord.push_back(static_cast<float>(fps));

	if (fpsRecord.size() > RECORD_SIZE)
	{
		fpsRecord.erase(fpsRecord.begin());
	}
}

bool UISettings::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse;
	ImGuiWindowClass windowClass;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	if (mainViewport->Size.x <= 0 || mainViewport->Size.y <= 0)
	{
		// La ventana principal está minimizada, no dibujar la ventana de configuración
		return false;
	}

	ImVec2 mainViewportPos = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(ImVec2(mainViewportPos.x, mainViewportPos.y), ImGuiCond_Appearing, ImVec2(0.5f, 0.8f));

	ImGui::SetNextWindowClass(&windowClass);
	windowClass.DockingAllowUnclassed = false;

	if (ImGui::Begin("Settings", &enabled, settingsFlags))
	{
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x * 0.2f, 20);
		ImVec2 performanceButtonPos = ImVec2(ImGui::GetWindowSize().x * 0.05f, ImGui::GetCursorPosY());
		ImVec2 hardwareButtonPos = ImVec2(ImGui::GetWindowSize().x * 0.05f, performanceButtonPos.y + buttonSize.y + 5);
		ImVec2 softwareButtonPos = ImVec2(ImGui::GetWindowSize().x * 0.05f, hardwareButtonPos.y + buttonSize.y + 5);
		ImVec2 editorCamButtonPos = ImVec2(ImGui::GetWindowSize().x * 0.05f, softwareButtonPos.y + buttonSize.y + 5);

		ImGui::SetCursorPos(performanceButtonPos);
		if (ImGui::Button("Performance", buttonSize))
		{
			settingType = SettingType::PERFORMANCE;
		}

		ImGui::SetCursorPos(hardwareButtonPos);
		if (ImGui::Button("Hardware", buttonSize))
		{
			settingType = SettingType::HARDWARE;
		}

		ImGui::SetCursorPos(softwareButtonPos);
		if (ImGui::Button("Software", buttonSize))
		{
			settingType = SettingType::SOFTWARE;
		}

		ImGui::SetCursorPos(editorCamButtonPos);
		if (ImGui::Button("Editor Camera", buttonSize))
		{
			settingType = SettingType::EDITORCAM;
		}

		ImGui::Dummy(ImVec2(0, 20));

		switch (settingType)
		{
		case SettingType::PERFORMANCE:
			PerformanceData();
			break;
		case SettingType::HARDWARE:
			HardwareData();
			break;
		case SettingType::SOFTWARE:
			SoftwareData();
			break;
		case SettingType::EDITORCAM:
			EditorCamData();
			break;
		}

	}
	ImGui::End();
	return true;
}

void UISettings::PerformanceData()
{
	int currentFps = Application->GetFps();

	ImGui::Separator();

	ImGui::Text("Performance Information");
	ImGui::PlotLines("FPS", fpsRecord.data(), static_cast<int>(fpsRecord.size()), 0, nullptr, 0.0f, 300.0f, ImVec2(0, 80));
}

void UISettings::HardwareData()
{
	HardwareSettings settings = Application->hardwareInfo->GetSettings();

	ImGui::Text("Hardware Information");
	ImGui::Separator();

	// CPU Information
	ImGui::Text("CPU:");
	ImGui::BulletText("Cores: %d", settings.cpuCores);
	ImGui::BulletText("Cache Line Size: %d", settings.cacheLineSize);
	ImGui::BulletText("RDTSC: %s", settings.rdtsc ? "Supported" : "Not Supported");
	ImGui::BulletText("AltiVec: %s", settings.altiVec ? "Supported" : "Not Supported");
	ImGui::BulletText("3DNow: %s", settings.now3d ? "Supported" : "Not Supported");
	ImGui::BulletText("MMX: %s", settings.mmx ? "Supported" : "Not Supported");
	ImGui::BulletText("SSE: %s", settings.sse ? "Supported" : "Not Supported");
	ImGui::BulletText("SSE2: %s", settings.sse2 ? "Supported" : "Not Supported");
	ImGui::BulletText("SSE3: %s", settings.sse3 ? "Supported" : "Not Supported");
	ImGui::BulletText("SSE4.1: %s", settings.sse41 ? "Supported" : "Not Supported");
	ImGui::BulletText("SSE4.2: %s", settings.sse42 ? "Supported" : "Not Supported");
	ImGui::BulletText("AVX: %s", settings.avx ? "Supported" : "Not Supported");
	ImGui::BulletText("AVX2: %s", settings.avx2 ? "Supported" : "Not Supported");

	ImGui::Separator();

	// Memory Information
	ImGui::Text("Memory:");
	ImGui::BulletText("RAM: %.2f GB", settings.ram);

	ImGui::Separator();

	// GPU Information
	ImGui::Text("GPU:");
	ImGui::BulletText("Vendor: %s", settings.gpuVendor.c_str());
	ImGui::BulletText("Brand: %s", settings.gpuBrand.c_str());
	ImGui::BulletText("Driver Version: %s", settings.gpuDriverVersion.c_str());

	ImGui::Separator();

	// VRAM Information
	if (settings.vramBudget > 0.0f) {
		ImGui::BulletText("Total VRAM Budget: %.2f MB", settings.vramBudget);
		ImGui::BulletText("Available VRAM: %.2f MB", settings.vramAvailable);
		ImGui::BulletText("VRAM Usage: %.2f MB", settings.vramUsage);
	}
	else {
		ImGui::BulletText("VRAM information not available");
	}

	ImGui::Separator();

}

void UISettings::SoftwareData()
{
	HardwareSettings settings = Application->hardwareInfo->GetSettings();

	ImGui::Text("Software Information");
	ImGui::Separator();

	// SDL Version
	ImGui::Text("SDL:");
	ImGui::BulletText("Version: %s", settings.sdlVersion);

	ImGui::Separator();

	// OpenGL Information
	ImGui::Text("OpenGL:");
	ImGui::BulletText("Vendor: %s", settings.gpuVendor.c_str());
	ImGui::BulletText("Renderer: %s", settings.gpuBrand.c_str());
	ImGui::BulletText("Version: %s", settings.gpuDriverVersion.c_str());

	ImGui::Separator();

	// Assimp Version
	ImGui::Text("Assimp:");
	ImGui::BulletText("Version: %d.%d.%d", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

	ImGui::Separator();

	// DevIL Version
	ImGui::Text("DevIL:");
	ImGui::BulletText("Version: %d.%d.%d",
		ilGetInteger(IL_VERSION_NUM) / 100,        // Major version
		(ilGetInteger(IL_VERSION_NUM) / 10) % 10,  // Minor version
		ilGetInteger(IL_VERSION_NUM) % 10          // Revision version
	);
}

void UISettings::EditorCamData()
{
	ImGui::Text("Editor Camera Settings");
	ImGui::Separator();

	EditorCamera* camera = Application->camera;
	
	bool orthographic = camera->IsOrthographic();

	// FOV
	if (orthographic)
	{
		float fov = camera->GetFOV();
		fov = glm::degrees(fov);
		if (ImGui::SliderFloat("FOV", &fov, 1.0f, 179.0f))
		{
			camera->SetFOV(glm::radians(fov));
		}
	}

	// Near Plane
	float zNear = static_cast<float>(camera->GetNearPlane());
	if (ImGui::SliderFloat("Near Plane", &zNear, 0.00001f, 100.0f))
	{
		camera->SetNearPlane(zNear);
	}

	// Far Plane
	float zFar = static_cast<float>(camera->GetFarPlane());
	if (ImGui::SliderFloat("Far Plane", &zFar, 100.0f, 10000.0f))
	{
		camera->SetFarPlane(zFar);
	}

	// Orthographic
	if (ImGui::Checkbox("Orthographic", &orthographic))
	{
		camera->SetOrthographic(orthographic);
	}

	// Ortho Size (solo si es ortográfica)
	if (orthographic)
	{
		float orthoSize = camera->GetOrthoSize();
		if (ImGui::SliderFloat("Ortho Size", &orthoSize, 1.0f, 100.0f))
		{
			camera->SetOrthoSize(orthoSize);
		}
	}

	// Movement Speed
	float movementSpeed = camera->GetMovementSpeed();
	if (ImGui::SliderFloat("Movement Speed", &movementSpeed, 0.1f, 10.0f))
	{
		camera->SetMovementSpeed(movementSpeed);
	}

	// Mouse Sensitivity
	float mouseSensitivity = camera->GetMouseSensitivity();
	if (ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f, 10.0f))
	{
		camera->SetMouseSensitivity(mouseSensitivity);
	}
}

void UISettings::ApplyLightTheme(ImGuiStyle& style)
{
	style.Alpha = 1.0f;
	style.FramePadding = ImVec2(4, 2);
	style.ItemSpacing = ImVec2(10, 2);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void UISettings::ApplyDarkTheme(ImGuiStyle& style)
{
	style.Alpha = 1.0f;
	style.FramePadding = ImVec2(4, 2);
	style.ItemSpacing = ImVec2(10, 2);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void UISettings::ApplyCustomTheme(ImGuiStyle& style) {
	style.Alpha = 1.0f;
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing = ImVec2(12, 4);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
}