#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../MyGameEngine/Shaders.h"
#include "../MyGameEngine/Material.h"
#include "../MyGameEngine/Mesh.h"

class GameObject;

struct InstanceData {
	glm::mat4 modelMatrix;
	glm::vec4 color;
};

struct RenderCommand {
	GameObject* gameObject;                  // Owner game object
	std::shared_ptr<Mesh> mesh;              // Mesh to render
	std::shared_ptr<Material> material;      // Material to apply
	glm::mat4 modelMatrix;                   // Transformation matrix
	float distanceToCamera;                  // Distance to camera for sorting
	bool isTransparent;                      // Whether this object is transparent
	unsigned int sortKey;                    // Key for sorting render commands

	struct {
		bool isAnimated;                     // Whether this mesh is animated
		std::vector<glm::mat4> boneMatrices; // Bone matrices for skeletal animation
		bool useLightData;                   // Whether to apply light data
	} specialData;

	void GenerateSortKey();

	static bool Compare(const RenderCommand& a, const RenderCommand& b);
};

struct RenderBatch {
	std::shared_ptr<Mesh> mesh;              // Shared mesh
	std::shared_ptr<Material> material;      // Shared material
	std::vector<RenderCommand> commands;     // Commands in this batch
	ShaderType shaderType;                   // Shader type for this batch
	bool isTransparent;                      // Whether this batch contains transparent objects

	void GenerateInstanceData(std::vector<InstanceData>& instanceData) const;

	void SortCommands() {
		if (isTransparent) {
			std::sort(commands.begin(), commands.end(),
				[](const RenderCommand& a, const RenderCommand& b) {
					return a.distanceToCamera > b.distanceToCamera;
				});
		}
		else {
			std::sort(commands.begin(), commands.end(),
				[](const RenderCommand& a, const RenderCommand& b) {
					return a.sortKey < b.sortKey;
				});
		}
	}
};

struct RenderQueue {
	std::vector<RenderCommand> commands;

	void AddCommand(const RenderCommand& command);

	void Sort();

	void Clear();

	bool IsEmpty() const;
};