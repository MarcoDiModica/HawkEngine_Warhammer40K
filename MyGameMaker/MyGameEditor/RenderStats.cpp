#include "RenderStats.h"
#include "App.h"
#include <algorithm>
#include <numeric>

extern App* Application;

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
	perfData.instanceCountHistory.resize(perfData.maxFrameHistory, 0.0f);

	settings.useForwardPlus = true;
	settings.useGPUCulling = true;
	settings.useFrustumCulling = true;
}

void RenderDebugPanel::Shutdown() {
}

void RenderDebugPanel::Reset() {
	perfData.minFrameTime = FLT_MAX;
	perfData.maxFrameTime = 0.0f;
	perfData.avgFrameTime = 0.0f;
	perfData.frameTimeAccum = 0.0f;

	perfData.minGpuTime = FLT_MAX;
	perfData.maxGpuTime = 0.0f;
	perfData.avgGpuTime = 0.0f;
	perfData.gpuTimeAccum = 0.0f;

	perfData.frameCounter = 0;

	std::fill(perfData.frameTimeHistory.begin(), perfData.frameTimeHistory.end(), 0.0f);
	std::fill(perfData.gpuTimeHistory.begin(), perfData.gpuTimeHistory.end(), 0.0f);
	std::fill(perfData.drawCallsHistory.begin(), perfData.drawCallsHistory.end(), 0.0f);
	std::fill(perfData.visibleObjectsHistory.begin(), perfData.visibleObjectsHistory.end(), 0.0f);
	std::fill(perfData.visibleLightsHistory.begin(), perfData.visibleLightsHistory.end(), 0.0f);
	std::fill(perfData.instanceCountHistory.begin(), perfData.instanceCountHistory.end(), 0.0f);
}

void RenderDebugPanel::Render() {
	if (!isVisible) return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Renderer Debug", &isVisible, flags)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Options")) {
				if (ImGui::MenuItem("Reset Statistics")) {
					Reset();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Close")) {
					isVisible = false;
				}

				ImGui::EndMenu();
			}

			float fps = Application ? Application->GetFps() : 0.0f;

			ImGui::SameLine(ImGui::GetWindowWidth() - 120);
			ImGui::Text("%.1f FPS", fps);

			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("SettingsTabs")) {
			if (ImGui::BeginTabItem("Statistics")) {
				currentTab = 3;
				RenderStatisticsPanel();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Performance")) {
				currentTab = 2;
				RenderPerformancePanel();
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("Settings")) {
				currentTab = 0;
				RenderGeneralSettings();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Forward+")) {
				currentTab = 1;
				RenderForwardPlusSettings();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void RenderDebugPanel::RenderGeneralSettings() {
	ImGui::BeginChild("GeneralSettings", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.3f, 0.5f, 0.8f, 1.0f), "Render System Settings");
	ImGui::Separator();

	const auto& stats = RenderManager::GetInstance().GetStatistics();

	if (ImGui::Checkbox("Use Forward+ Lighting", &settings.useForwardPlus)) {
		RenderManager::GetInstance().SetUseForwardPlus(settings.useForwardPlus);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("Forward+ allows hundreds of lights with\ntile-based light culling.");
		ImGui::EndTooltip();
	}

	if (ImGui::Checkbox("Use GPU Culling", &settings.useGPUCulling)) {
		RenderManager::GetInstance().SetUseGPUCulling(settings.useGPUCulling);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("Offloads object culling to the GPU for\nbetter performance with many objects.");
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
		ImGui::Text("Hi-Z based occlusion culling.\nRequires GPU with all required extensions.");
		ImGui::EndTooltip();
	}

	ImGui::EndChild();
}

void RenderDebugPanel::RenderForwardPlusSettings() {
	ImGui::BeginChild("ForwardPlusSettings", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.2f, 1.0f), "Forward+ Lighting Settings");
	ImGui::Separator();

	if (!settings.useForwardPlus) {
		ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "Forward+ Lighting is disabled.");
		ImGui::Text("Enable it in the 'Settings' tab to configure.");
		ImGui::EndChild();
		return;
	}

	ImGui::Text("Tile Size: %d x %d", settings.tileSize, settings.tileSize);
	ImGui::Text("Screen Tiles: %d x %d = %d tiles",
		ForwardPlusLighting::GetInstance().GetTilesX(),
		ForwardPlusLighting::GetInstance().GetTilesY(),
		ForwardPlusLighting::GetInstance().GetTilesX() * ForwardPlusLighting::GetInstance().GetTilesY());
	ImGui::Text("Max Lights per Tile: %d", settings.maxLightsPerTile);

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

	ImGui::EndChild();
}

void RenderDebugPanel::RenderPerformancePanel() {
	ImGui::BeginChild("PerformancePanel", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.2f, 0.7f, 0.4f, 1.0f), "Performance Monitoring");
	ImGui::Separator();

	ImVec2 graphSize(ImGui::GetContentRegionAvail().x, 80);

	// Frame Time
	{
		ImGui::Text("Frame Time (ms)");

		float fps = perfData.avgFrameTime > 0.0f ? 1000.0f / perfData.avgFrameTime : 0.0f;

		ImGui::Text("Avg: %.2f ms (%.1f FPS)   Min: %.2f ms   Max: %.2f ms",
			perfData.avgFrameTime, fps,
			perfData.minFrameTime == FLT_MAX ? 0.0f : perfData.minFrameTime,
			perfData.maxFrameTime);

		DrawHistoryGraph("##FrameTimeGraph", perfData.frameTimeHistory, 0,
			perfData.maxFrameTime * 1.2f + 0.1f, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
	}

	ImGui::Separator();

	// GPU Time
	{
		ImGui::Text("GPU Time (ms)");

		ImGui::Text("Avg: %.2f ms   Min: %.2f ms   Max: %.2f ms",
			perfData.avgGpuTime,
			perfData.minGpuTime == FLT_MAX ? 0.0f : perfData.minGpuTime,
			perfData.maxGpuTime);

		DrawHistoryGraph("##GPUTimeGraph", perfData.gpuTimeHistory, 0,
			perfData.maxGpuTime * 1.2f + 0.1f, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
	}

	ImGui::EndChild();
}

void RenderDebugPanel::RenderStatisticsPanel() {
	ImGui::BeginChild("StatisticsPanel", ImVec2(0, 0), true);

	ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.8f, 1.0f), "Renderer Statistics");
	ImGui::Separator();

	const auto& stats = RenderManager::GetInstance().GetStatistics();

	ImGui::Columns(2, "StatColumns", false);

	ImGui::Text("Total GameObjects:"); ImGui::NextColumn();
	ImGui::Text("%d", stats.totalGameObjects); ImGui::NextColumn();

	ImGui::Text("Visible GameObjects:"); ImGui::NextColumn();
	ImGui::Text("%d (%.1f%%)", stats.visibleGameObjects,
		stats.totalGameObjects > 0 ? (100.0f * stats.visibleGameObjects / stats.totalGameObjects) : 0.0f);
	ImGui::NextColumn();

	ImGui::Text("Total Draw Calls:"); ImGui::NextColumn();
	ImGui::Text("%d", stats.totalDrawCalls); ImGui::NextColumn();

	ImGui::Text("Total Lights:"); ImGui::NextColumn();
	ImGui::Text("%d", stats.totalLights); ImGui::NextColumn();

	ImGui::Text("Visible Lights:"); ImGui::NextColumn();
	ImGui::Text("%d (%.1f%%)", stats.visibleLights,
		stats.totalLights > 0 ? (100.0f * stats.visibleLights / stats.totalLights) : 0.0f);
	ImGui::NextColumn();

	ImGui::Text("Instance Count:"); ImGui::NextColumn();
	ImGui::Text("%d", BindlessManager::GetInstance().GetInstanceCount()); ImGui::NextColumn();

	ImGui::Text("Frame Time:"); ImGui::NextColumn();
	ImGui::Text("%.2f ms (%.1f FPS)", perfData.avgFrameTime,
		perfData.avgFrameTime > 0 ? 1000.0f / perfData.avgFrameTime : 0.0f);
	ImGui::NextColumn();

	ImGui::Text("GPU Time:"); ImGui::NextColumn();
	ImGui::Text("%.2f ms", perfData.avgGpuTime); ImGui::NextColumn();

	ImGui::Columns(1);

	ImGui::Separator();

	ImGui::Text("Draw Calls History");
	DrawHistoryGraph("##DrawCallsGraph", perfData.drawCallsHistory, 0, 0,
		ImVec4(0.4f, 0.4f, 0.8f, 1.0f));

	ImGui::Text("Visible Objects History");
	DrawHistoryGraph("##VisibleObjectsGraph", perfData.visibleObjectsHistory, 0, 0,
		ImVec4(0.4f, 0.8f, 0.4f, 1.0f));

	ImGui::Text("Visible Lights History");
	DrawHistoryGraph("##VisibleLightsGraph", perfData.visibleLightsHistory, 0, 0,
		ImVec4(0.8f, 0.8f, 0.2f, 1.0f));

	ImGui::EndChild();
}

void RenderDebugPanel::DrawHistoryGraph(const char* label, const std::vector<float>& data,
	float minScale, float maxScale, ImVec4 color) {
	if (data.empty()) return;

	if (minScale == 0 && maxScale == 0) {
		auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end());
		minScale = *minIt;
		maxScale = *maxIt * 1.2f;

		if (maxScale <= minScale + 0.001f) {
			maxScale = minScale + 1.0f;
		}
	}

	ImGui::PlotLines(label, data.data(), static_cast<int>(data.size()), 0, nullptr,
		minScale, maxScale, ImVec2(ImGui::GetContentRegionAvail().x, 60));
}

void RenderDebugPanel::UpdateStatistics(float frameTimeMs, float gpuTimeMs) {
	const auto& stats = RenderManager::GetInstance().GetStatistics();

	perfData.minFrameTime = std::min(perfData.minFrameTime, frameTimeMs);
	perfData.maxFrameTime = std::max(perfData.maxFrameTime, frameTimeMs);
	perfData.frameTimeAccum += frameTimeMs;

	perfData.minGpuTime = std::min(perfData.minGpuTime, gpuTimeMs);
	perfData.maxGpuTime = std::max(perfData.maxGpuTime, gpuTimeMs);
	perfData.gpuTimeAccum += gpuTimeMs;

	perfData.frameCounter++;

	if (perfData.frameCounter > 0) {
		perfData.avgFrameTime = perfData.frameTimeAccum / perfData.frameCounter;
		perfData.avgGpuTime = perfData.gpuTimeAccum / perfData.frameCounter;
	}

	if (perfData.frameCounter >= perfData.statsResetInterval) {
		perfData.minFrameTime = FLT_MAX;
		perfData.maxFrameTime = 0.0f;
		perfData.frameTimeAccum = 0.0f;

		perfData.minGpuTime = FLT_MAX;
		perfData.maxGpuTime = 0.0f;
		perfData.gpuTimeAccum = 0.0f;

		perfData.frameCounter = 0;
	}

	if (!perfData.frameTimeHistory.empty()) {
		perfData.frameTimeHistory.erase(perfData.frameTimeHistory.begin());
	}
	perfData.frameTimeHistory.push_back(frameTimeMs);

	if (!perfData.gpuTimeHistory.empty()) {
		perfData.gpuTimeHistory.erase(perfData.gpuTimeHistory.begin());
	}
	perfData.gpuTimeHistory.push_back(gpuTimeMs);

	if (!perfData.drawCallsHistory.empty()) {
		perfData.drawCallsHistory.erase(perfData.drawCallsHistory.begin());
	}
	perfData.drawCallsHistory.push_back(static_cast<float>(stats.totalDrawCalls));

	if (!perfData.visibleObjectsHistory.empty()) {
		perfData.visibleObjectsHistory.erase(perfData.visibleObjectsHistory.begin());
	}
	perfData.visibleObjectsHistory.push_back(static_cast<float>(stats.visibleGameObjects));

	if (!perfData.visibleLightsHistory.empty()) {
		perfData.visibleLightsHistory.erase(perfData.visibleLightsHistory.begin());
	}
	perfData.visibleLightsHistory.push_back(static_cast<float>(stats.visibleLights));

	if (!perfData.instanceCountHistory.empty()) {
		perfData.instanceCountHistory.erase(perfData.instanceCountHistory.begin());
	}
	perfData.instanceCountHistory.push_back(static_cast<float>(BindlessManager::GetInstance().GetInstanceCount()));
}