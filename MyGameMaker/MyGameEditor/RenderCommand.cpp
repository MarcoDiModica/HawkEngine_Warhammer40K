#include "RenderCommand.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/MeshRendererComponent.h"

void RenderCommand::GenerateSortKey() {

	unsigned int materialID = material ? material->GetId() : 0;
	unsigned int meshID = mesh ? mesh->getModel()->GetID() : 0;

	unsigned int distanceKey = isTransparent
		? 0xFFFFFFFF - static_cast<unsigned int>(distanceToCamera * 1000.0f)
		: static_cast<unsigned int>(distanceToCamera * 1000.0f);

	sortKey = (materialID << 24) | (meshID << 16) | (distanceKey & 0xFFFF);
}

bool RenderCommand::Compare(const RenderCommand& a, const RenderCommand& b) {
	if (a.isTransparent && b.isTransparent) {
		return a.distanceToCamera > b.distanceToCamera;
	}

	return a.sortKey < b.sortKey;
}

void RenderBatch::GenerateInstanceData(std::vector<InstanceData>& instanceData) const {
	instanceData.clear();
	instanceData.reserve(commands.size());

	for (const auto& command : commands) {
		InstanceData data;
		data.modelMatrix = command.modelMatrix;

		if (command.material) {
			data.color = command.material->GetColor();
		}
		else {
			data.color = glm::vec4(1.0f);
		}

		instanceData.push_back(data);
	}
}

void RenderQueue::AddCommand(const RenderCommand& command) {
	commands.push_back(command);
}

void RenderQueue::Sort() {
	for (auto& command : commands) {
		command.GenerateSortKey();
	}

	std::sort(commands.begin(), commands.end(),
		[](const RenderCommand& a, const RenderCommand& b) {
			return RenderCommand::Compare(a, b);
		});
}

void RenderQueue::Clear() {
	commands.clear();
}

bool RenderQueue::IsEmpty() const {
	return commands.empty();
}