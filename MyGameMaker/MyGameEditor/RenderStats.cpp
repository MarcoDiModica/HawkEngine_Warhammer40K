#include "RenderStats.h"
#include <sstream>
#include <iomanip>
#include <iostream>

RenderStats& RenderStats::GetInstance() {
	static RenderStats instance;
	return instance;
}

RenderStats::RenderStats()
	: drawCallsStandard(0)
	, drawCallsInstanced(0)
	, totalObjectsRendered(0)
	, totalTrianglesRendered(0)
	, shaderChanges(0)
	, textureBindings(0)
	, materialChanges(0)
	, vaoBindings(0)
	, bufferMemoryUpdated(0)
	, verbosityLevel(VerbosityLevel::BASIC) {
}

void RenderStats::Reset() {
	drawCallsStandard = 0;
	drawCallsInstanced = 0;
	totalObjectsRendered = 0;
	totalTrianglesRendered = 0;
	shaderChanges = 0;
	textureBindings = 0;
	materialChanges = 0;
	vaoBindings = 0;
	bufferMemoryUpdated = 0;

	sectionTimes.clear();
	currentBatchTag.clear();
}

void RenderStats::RecordDrawCall(bool instanced, int instanceCount, int triangleCount) {
	if (instanced) {
		drawCallsInstanced++;
		totalObjectsRendered += instanceCount;
	}
	else {
		drawCallsStandard++;
		totalObjectsRendered++;
	}

	totalTrianglesRendered += triangleCount * (instanced ? instanceCount : 1);

	if (verbosityLevel >= VerbosityLevel::PERFRAME) {
		std::stringstream ss;
		ss << "DrawCall: " << (instanced ? "Instanced (" + std::to_string(instanceCount) + ")" : "Standard")
			<< " Triangles: " << triangleCount * (instanced ? instanceCount : 1);

		if (!currentBatchTag.empty()) {
			ss << " Batch: " << currentBatchTag;
		}

		std::cout << ss.str() << std::endl;
	}
}

void RenderStats::RecordShaderChange() {
	shaderChanges++;

	if (verbosityLevel >= VerbosityLevel::FULL) {
		std::cout << "Shader change #" << shaderChanges << std::endl;
	}
}

void RenderStats::RecordTextureBinding() {
	textureBindings++;

	if (verbosityLevel >= VerbosityLevel::FULL) {
		std::cout << "Texture binding #" << textureBindings << std::endl;
	}
}

void RenderStats::RecordMaterialChange() {
	materialChanges++;

	if (verbosityLevel >= VerbosityLevel::FULL) {
		std::cout << "Material change #" << materialChanges << std::endl;
	}
}

void RenderStats::RecordVAOBinding() {
	vaoBindings++;

	if (verbosityLevel >= VerbosityLevel::FULL) {
		std::cout << "VAO binding #" << vaoBindings << std::endl;
	}
}

void RenderStats::RecordBufferUpdate(size_t bytes) {
	bufferMemoryUpdated += bytes;

	if (verbosityLevel >= VerbosityLevel::FULL) {
		std::cout << "Buffer update: " << bytes << " bytes" << std::endl;
	}
}

void RenderStats::StartTimingSection(const std::string& section) {
	sectionStartTimes[section] = std::chrono::high_resolution_clock::now();
}

void RenderStats::EndTimingSection(const std::string& section) {
	auto endTime = std::chrono::high_resolution_clock::now();

	auto it = sectionStartTimes.find(section);
	if (it != sectionStartTimes.end()) {
		auto startTime = it->second;
		float duration = std::chrono::duration<float, std::milli>(endTime - startTime).count();
		sectionTimes[section] = duration;

		if (verbosityLevel >= VerbosityLevel::DETAILED) {
			std::cout << "Section '" << section << "' took " << duration << " ms" << std::endl;
		}
	}
}

void RenderStats::BeginBatch(const std::string& tag) {
	currentBatchTag = tag;

	if (verbosityLevel >= VerbosityLevel::DETAILED) {
		std::cout << "--- Begin batch: " << tag << " ---" << std::endl;
	}
}

void RenderStats::EndBatch() {
	if (verbosityLevel >= VerbosityLevel::DETAILED && !currentBatchTag.empty()) {
		std::cout << "--- End batch: " << currentBatchTag << " ---" << std::endl;
	}

	currentBatchTag.clear();
}

int RenderStats::GetTotalDrawCalls() const {
	return drawCallsStandard + drawCallsInstanced;
}

int RenderStats::GetInstancedDrawCalls() const {
	return drawCallsInstanced;
}

int RenderStats::GetStandardDrawCalls() const {
	return drawCallsStandard;
}

int RenderStats::GetTotalObjectsRendered() const {
	return totalObjectsRendered;
}

int RenderStats::GetTotalTrianglesRendered() const {
	return totalTrianglesRendered;
}

int RenderStats::GetShaderChanges() const {
	return shaderChanges;
}

int RenderStats::GetTextureBindings() const {
	return textureBindings;
}

int RenderStats::GetMaterialChanges() const {
	return materialChanges;
}

int RenderStats::GetVAOBindings() const {
	return vaoBindings;
}

size_t RenderStats::GetTotalBufferMemoryUpdated() const {
	return bufferMemoryUpdated;
}

float RenderStats::GetSectionTime(const std::string& section) const {
	auto it = sectionTimes.find(section);
	if (it != sectionTimes.end()) {
		return it->second;
	}
	return 0.0f;
}

std::vector<std::pair<std::string, float>> RenderStats::GetAllSectionTimes() const {
	std::vector<std::pair<std::string, float>> result;
	result.reserve(sectionTimes.size());

	for (const auto& pair : sectionTimes) {
		result.emplace_back(pair.first, pair.second);
	}

	return result;
}

std::string RenderStats::GetStatsReport() const {
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2);

	ss << "===== RENDER STATISTICS =====\n";
	ss << "Draw Calls: " << GetTotalDrawCalls()
		<< " (Standard: " << drawCallsStandard
		<< ", Instanced: " << drawCallsInstanced << ")\n";
	ss << "Objects Rendered: " << totalObjectsRendered << "\n";
	ss << "Triangles Rendered: " << totalTrianglesRendered << "\n";
	ss << "State Changes: [Shader: " << shaderChanges
		<< ", Material: " << materialChanges
		<< ", VAO: " << vaoBindings
		<< ", Texture: " << textureBindings << "]\n";
	ss << "Buffer Memory Updated: " << (bufferMemoryUpdated / 1024.0f) << " KB\n";

	ss << "--- Timing Information ---\n";
	for (const auto& pair : sectionTimes) {
		ss << pair.first << ": " << pair.second << " ms\n";
	}
	ss << "============================\n";

	return ss.str();
}

void RenderStats::SetVerbosityLevel(VerbosityLevel level) {
	verbosityLevel = level;
}

RenderStats::VerbosityLevel RenderStats::GetVerbosityLevel() const {
	return verbosityLevel;
}

void RenderStats::SetupOpenGLDebugCallback() {
	if (GLEW_ARB_debug_output || GLEW_KHR_debug) {
		std::cout << "OpenGL debug callback enabled" << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback((GLDEBUGPROC)DebugCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}
	else {
		std::cout << "OpenGL debug callback not supported" << std::endl;
	}
}

void RenderStats::DebugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::string sourceStr;
	switch (source) {
	case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
	default:                              sourceStr = "Unknown"; break;
	}

	std::string typeStr;
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behavior"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
	default:                                typeStr = "Unknown"; break;
	}

	std::string severityStr;
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:         severityStr = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          severityStr = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "Notification"; break;
	default:                             severityStr = "Unknown"; break;
	}

	std::cout << "OpenGL Debug: [" << severityStr << "] " << typeStr
		<< " (" << sourceStr << ") " << id << ": " << message << std::endl;
}