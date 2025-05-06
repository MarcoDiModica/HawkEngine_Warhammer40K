#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "RenderManager.h"
#include "ForwardPlus.h"
#include "GPUDrivenRenderer.h"
#include "BindlessManager.h"
#include "imgui.h"

class RenderDebugPanel {
public:
	static RenderDebugPanel& GetInstance();

	void Initialize();
	void Shutdown();
	void Render();
	void Reset();
	void UpdateStatistics(float frameTimeMs, float gpuTimeMs);

	void SetVisible(bool visible) { isVisible = visible; }
	bool IsVisible() const { return isVisible; }
	void ToggleVisibility() { isVisible = !isVisible; }

private:
	RenderDebugPanel() = default;
	~RenderDebugPanel() = default;
	RenderDebugPanel(const RenderDebugPanel&) = delete;
	RenderDebugPanel& operator=(const RenderDebugPanel&) = delete;

	void RenderGeneralSettings();
	void RenderForwardPlusSettings();
	void RenderPerformancePanel();
	void RenderStatisticsPanel();

	void DrawHistoryGraph(const char* label, const std::vector<float>& data,
		float minScale, float maxScale, ImVec4 color);

	bool isVisible = true;
	int currentTab = 0;

	struct Settings {
		bool useForwardPlus = true;
		bool useGPUCulling = true;
		bool useFrustumCulling = true;
		bool useOcclusionCulling = false;
		int tileSize = 16;
		int maxLightsPerTile = 64;
	} settings;

	struct PerformanceData {
		std::vector<float> frameTimeHistory;
		std::vector<float> gpuTimeHistory;
		std::vector<float> drawCallsHistory;
		std::vector<float> visibleObjectsHistory;
		std::vector<float> visibleLightsHistory;
		std::vector<float> instanceCountHistory;

		float minFrameTime = FLT_MAX;
		float maxFrameTime = 0.0f;
		float avgFrameTime = 0.0f;
		float frameTimeAccum = 0.0f;

		float minGpuTime = FLT_MAX;
		float maxGpuTime = 0.0f;
		float avgGpuTime = 0.0f;
		float gpuTimeAccum = 0.0f;

		int maxFrameHistory = 120;
		int frameCounter = 0;
		int statsResetInterval = 60;
	} perfData;
};