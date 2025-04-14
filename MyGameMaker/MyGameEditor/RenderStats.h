#pragma once

#include <vector>
#include <deque>
#include <string>
#include "RenderManager.h"
#include "ForwardPlus.h"
#include "GPUDrivenRenderer.h"

class RenderDebugPanel {
public:
	static RenderDebugPanel& GetInstance();

	void Initialize();
	void Shutdown();

	void Render();

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
	void RenderPerformanceGraph();
	void RenderDebugSettings();

	void UpdateStatistics();

	bool isVisible = true;
	int currentTab = 0;

	struct Settings {
		bool useForwardPlus = true;
		bool useGPUCulling = true;
		bool useFrustumCulling = true;
		bool useOcclusionCulling = false;
		int tileSize = 16;
		int maxLightsPerTile = 64;
		bool showBoundingSpheres = false;
		bool showLightVolumes = false;
		bool showTileGrid = false;
	} settings;

	struct PerformanceData {
		std::deque<float> frameTimeHistory;
		std::deque<float> gpuTimeHistory;
		std::deque<float> drawCallsHistory;
		std::deque<float> visibleObjectsHistory;
		std::deque<float> visibleLightsHistory;
		int maxFrameHistory = 120;
	} perfData;
};