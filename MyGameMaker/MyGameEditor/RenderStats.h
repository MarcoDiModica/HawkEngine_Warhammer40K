#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <GL/glew.h>

#ifndef GLDEBUGPROC
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
typedef void (APIENTRY* GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam);
#endif

class RenderStats {
public:
	static RenderStats& GetInstance();

	void Reset();

	void RecordDrawCall(bool instanced, int instanceCount, int triangleCount);
	void RecordShaderChange();
	void RecordTextureBinding();
	void RecordMaterialChange();
	void RecordVAOBinding();
	void RecordBufferUpdate(size_t bytes);

	void StartTimingSection(const std::string& section);
	void EndTimingSection(const std::string& section);

	void BeginBatch(const std::string& tag);
	void EndBatch();

	int GetTotalDrawCalls() const;
	int GetInstancedDrawCalls() const;
	int GetStandardDrawCalls() const;
	int GetTotalObjectsRendered() const;
	int GetTotalTrianglesRendered() const;
	int GetShaderChanges() const;
	int GetTextureBindings() const;
	int GetMaterialChanges() const;
	int GetVAOBindings() const;
	size_t GetTotalBufferMemoryUpdated() const;

	float GetSectionTime(const std::string& section) const;
	std::vector<std::pair<std::string, float>> GetAllSectionTimes() const;

	std::string GetStatsReport() const;

	enum class VerbosityLevel {
		NONE,
		BASIC,
		DETAILED,
		PERFRAME,
		FULL
	};

	void SetVerbosityLevel(VerbosityLevel level);
	VerbosityLevel GetVerbosityLevel() const;

	static void SetupOpenGLDebugCallback();

	static void DebugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* userParam);

private:
	RenderStats();
	~RenderStats() = default;

	RenderStats(const RenderStats&) = delete;
	RenderStats& operator=(const RenderStats&) = delete;

	int drawCallsStandard;
	int drawCallsInstanced;
	int totalObjectsRendered;
	int totalTrianglesRendered;
	int shaderChanges;
	int textureBindings;
	int materialChanges;
	int vaoBindings;
	size_t bufferMemoryUpdated;

	std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> sectionStartTimes;
	std::unordered_map<std::string, float> sectionTimes; 

	std::string currentBatchTag;

	VerbosityLevel verbosityLevel;
};

#define RENDER_STATS RenderStats::GetInstance()
#define START_TIMING(section) RenderStats::GetInstance().StartTimingSection(section)
#define END_TIMING(section) RenderStats::GetInstance().EndTimingSection(section)
#define RECORD_DRAWCALL(instanced, count, triangles) RenderStats::GetInstance().RecordDrawCall(instanced, count, triangles)