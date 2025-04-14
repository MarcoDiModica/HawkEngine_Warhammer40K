#include "RenderStats.h"
#include "imgui.h"
#include <iostream>
#include <algorithm>
#include <chrono>

RenderDebugPanel& RenderDebugPanel::GetInstance() {
	static RenderDebugPanel instance;
	return instance;
}

void RenderDebugPanel::Initialize() {
	perfData.frameTimeHistory.resize(perfData.maxFrameHistory, 0.0f);
	perfData.gpuTimeHistory.resize(perfData.maxFrameHistory, 0.0f);
	perfData.drawCallsHistory.resize(perfData.maxFrameHistory, 0.0f);
	perfData.visibleObjectsHistory.resize(perfData.maxFrameHistory, 0.0f);
	perfData.visibleLightsHistory.resize(perfData.maxFrameHistory, 0.0f);

	settings.useForwardPlus = true;
	//settings.useGPUCulling = GPUDrivenRenderer::GetInstance().SetUseGPUCulling(true);
	settings.useFrustumCulling = true;
}

void RenderDebugPanel::Shutdown() {
}

void RenderDebugPanel::Render() {
	if (!isVisible) return;

	UpdateStatistics();

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Render System Debug", &isVisible, flags)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Clear Statistics")) {
					std::fill(perfData.frameTimeHistory.begin(), perfData.frameTimeHistory.end(), 0.0f);
					std::fill(perfData.gpuTimeHistory.begin(), perfData.gpuTimeHistory.end(), 0.0f);
					std::fill(perfData.drawCallsHistory.begin(), perfData.drawCallsHistory.end(), 0.0f);
					std::fill(perfData.visibleObjectsHistory.begin(), perfData.visibleObjectsHistory.end(), 0.0f);
					std::fill(perfData.visibleLightsHistory.begin(), perfData.visibleLightsHistory.end(), 0.0f);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close")) {
					isVisible = false;
				}

				ImGui::EndMenu();
			}

			const auto& stats = RenderManager::GetInstance().GetStatistics();
			float frameTime = stats.frameTimeMs;
			float fps = frameTime > 0 ? 1000.0f / frameTime : 0.0f;

			/*char fpsText[32];
			sprintf(fpsText, "%.1f FPS (%.2f ms)", fps, frameTime);

			float textWidth = ImGui::CalcTextSize(fpsText).x;
			ImGui::SameLine(ImGui::GetWindowWidth() - textWidth - 20);
			ImGui::Text("%s", fpsText);*/

			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("SettingsTabs")) {
			if (ImGui::BeginTabItem("General")) {
				currentTab = 0;
				RenderGeneralSettings();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Forward+")) {
				currentTab = 1;
				RenderForwardPlusSettings();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Performance")) {
				currentTab = 2;
				RenderPerformanceGraph();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Debug")) {
				currentTab = 3;
				RenderDebugSettings();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void RenderDebugPanel::RenderGeneralSettings() {
	ImGui::BeginChild("GeneralSettings", ImVec2(0, 0), true);

	// Título
	ImGui::TextColored(ImVec4(0.3f, 0.5f, 0.8f, 1.0f), "Render System Settings");
	ImGui::Separator();

	const auto& stats = RenderManager::GetInstance().GetStatistics();

	ImGui::Text("Total GameObjects: %d", stats.totalGameObjects);
	ImGui::Text("Visible GameObjects: %d", stats.visibleGameObjects);
	ImGui::Text("Total Draw Calls: %d", stats.totalDrawCalls);
	ImGui::Text("Total Lights: %d", stats.totalLights);
	ImGui::Text("Visible Lights: %d", stats.visibleLights);

	ImGui::Separator();

	if (ImGui::Checkbox("Use Forward+ Lighting", &settings.useForwardPlus)) {
		RenderManager::GetInstance().SetUseForwardPlus(settings.useForwardPlus);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("Forward+ permite cientos de luces\ncon culling de luces por tile.");
		ImGui::EndTooltip();
	}

	if (ImGui::Checkbox("Use GPU Culling", &settings.useGPUCulling)) {
		RenderManager::GetInstance().SetUseGPUCulling(settings.useGPUCulling);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("Delega culling de objetos a la GPU\npara mejor rendimiento con muchos objetos.");
		ImGui::EndTooltip();
	}

	if (ImGui::Checkbox("Use Frustum Culling", &settings.useFrustumCulling)) {
		GPUDrivenRenderer::GetInstance().SetUseFrustumCulling(settings.useFrustumCulling);
	}

	if (ImGui::Checkbox("Use Occlusion Culling", &settings.useOcclusionCulling)) {
		RenderManager::GetInstance().SetUseOcclusionCulling(settings.useOcclusionCulling);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("Culling de oclusión basado en Hi-Z.\nRequiere GPU con todas las extensiones.");
		ImGui::EndTooltip();
	}

	ImGui::EndChild();
}

void RenderDebugPanel::RenderForwardPlusSettings() {
	ImGui::BeginChild("ForwardPlusSettings", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.2f, 1.0f), "Forward+ Lighting Settings");
	ImGui::Separator();

	if (!settings.useForwardPlus) {
		ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "Forward+ Lighting está desactivado.");
		ImGui::Text("Actívalo en la pestaña 'General' para configurar.");
		ImGui::EndChild();
		return;
	}

	ImGui::Text("Tile Size: %d x %d", settings.tileSize, settings.tileSize);
	ImGui::Text("Screen Tiles: %d x %d = %d tiles",
		ForwardPlusLighting::GetInstance().GetTilesX(),
		ForwardPlusLighting::GetInstance().GetTilesY(),
		ForwardPlusLighting::GetInstance().GetTilesX() * ForwardPlusLighting::GetInstance().GetTilesY());
	ImGui::Text("Max Lights per Tile: %d", settings.maxLightsPerTile);
	ImGui::Text("Total Lights: %d", ForwardPlusLighting::GetInstance().GetTotalLights());
	ImGui::Text("Visible Lights: %d", ForwardPlusLighting::GetInstance().GetVisibleLights());
	ImGui::Text("Culled Lights: %d", ForwardPlusLighting::GetInstance().GetCulledLights());

	ImGui::Separator();

	int tileSize = settings.tileSize;
	if (ImGui::SliderInt("Tile Size", &tileSize, 8, 64)) {
		tileSize = tileSize & ~0x7;
		if (tileSize < 8) tileSize = 8;

		settings.tileSize = tileSize;
		ForwardPlusLighting::GetInstance().SetTileSize(tileSize);
	}

	int maxLights = settings.maxLightsPerTile;
	if (ImGui::SliderInt("Max Lights Per Tile", &maxLights, 16, 256)) {
		settings.maxLightsPerTile = maxLights;
		ForwardPlusLighting::GetInstance().SetMaxLightsPerTile(maxLights);
	}

	ImGui::Separator();

	ImGui::Checkbox("Show Tile Grid", &settings.showTileGrid);
	ImGui::Checkbox("Show Light Volumes", &settings.showLightVolumes);

	ImGui::EndChild();
}

void RenderDebugPanel::RenderPerformanceGraph() {
	ImGui::BeginChild("PerformanceGraphs", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.2f, 0.7f, 0.4f, 1.0f), "Performance Monitoring");
	ImGui::Separator();

	ImVec2 graphSize(ImGui::GetContentRegionAvail().x, 80);

	ImGui::Text("Frame Time (ms)");
	

	float avgFrameTime = 0.0f, minFrameTime = FLT_MAX, maxFrameTime = 0.0f;
	for (float time : perfData.frameTimeHistory) {
		avgFrameTime += time;
		minFrameTime = std::min(minFrameTime, time);
		maxFrameTime = std::max(maxFrameTime, time);
	}
	avgFrameTime /= perfData.frameTimeHistory.size();

	ImGui::Text("Avg: %.2f ms (%.1f FPS)   Min: %.2f ms   Max: %.2f ms",
		avgFrameTime, 1000.0f / avgFrameTime,
		minFrameTime, maxFrameTime);

	ImGui::Separator();

	ImGui::Text("GPU Time (ms)");

	float avgGPUTime = 0.0f, minGPUTime = FLT_MAX, maxGPUTime = 0.0f;
	for (float time : perfData.gpuTimeHistory) {
		avgGPUTime += time;
		minGPUTime = std::min(minGPUTime, time);
		maxGPUTime = std::max(maxGPUTime, time);
	}
	avgGPUTime /= perfData.gpuTimeHistory.size();

	ImGui::Text("Avg: %.2f ms   Min: %.2f ms   Max: %.2f ms",
		avgGPUTime, minGPUTime, maxGPUTime);

	ImGui::Separator();

	ImGui::Text("Draw Calls");
	

	ImGui::Separator();

	ImGui::Text("Visible Objects");
	

	ImGui::Separator();

	ImGui::Text("Visible Lights");

	ImGui::EndChild();
}

void RenderDebugPanel::RenderDebugSettings() {
	ImGui::BeginChild("DebugSettings", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.6f, 1.0f), "Debug Visualization");
	ImGui::Separator();

	ImGui::Checkbox("Show Bounding Spheres", &settings.showBoundingSpheres);
	ImGui::Checkbox("Show Light Volumes", &settings.showLightVolumes);
	ImGui::Checkbox("Show Tile Grid", &settings.showTileGrid);

	ImGui::Separator();

	static int debugViewMode = 0;
	const char* debugModes[] = {
		"Normal", "Wireframe", "Albedo", "Normals", "Metallic",
		"Roughness", "AO", "Light Count", "Depth"
	};

	ImGui::Text("Debug View Mode:");
	ImGui::Combo("##DebugViewMode", &debugViewMode, debugModes, IM_ARRAYSIZE(debugModes));

	// Aquí se implementaría código para aplicar modo seleccionado
	// ...

	ImGui::EndChild();
}

void RenderDebugPanel::UpdateStatistics() {
	const auto& stats = RenderManager::GetInstance().GetStatistics();

	perfData.frameTimeHistory.pop_front();
	perfData.frameTimeHistory.push_back(stats.frameTimeMs);

	perfData.gpuTimeHistory.pop_front();
	perfData.gpuTimeHistory.push_back(stats.gpuTimeMs);

	perfData.drawCallsHistory.pop_front();
	perfData.drawCallsHistory.push_back(static_cast<float>(stats.totalDrawCalls));

	perfData.visibleObjectsHistory.pop_front();
	perfData.visibleObjectsHistory.push_back(static_cast<float>(stats.visibleGameObjects));

	perfData.visibleLightsHistory.pop_front();
	perfData.visibleLightsHistory.push_back(static_cast<float>(stats.visibleLights));
}