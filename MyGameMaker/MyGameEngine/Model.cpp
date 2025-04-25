#include "Model.h"
#include "AssimpGLMHelpers.h"
#include <stdexcept>
#include <GL/glew.h>

uint32_t Model::nextModelID = 1;

Model::Model() {
}

Model::~Model() {
}

void Model::GenerateUniqueID() {
	m_ID = nextModelID++;

	if (nextModelID == 0) {
		nextModelID = 1;
	}
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex) {
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight) {
	if (weight < 0.0001f) {
		return;
	}

	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
		if (vertex.m_BoneIDs[i] < 0 || vertex.m_Weights[i] < weight) {
			for (int j = MAX_BONE_INFLUENCE - 1; j > i; --j) {
				vertex.m_BoneIDs[j] = vertex.m_BoneIDs[j - 1];
				vertex.m_Weights[j] = vertex.m_Weights[j - 1];
			}

			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}

	float sum = 0.0f;
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
		if (vertex.m_BoneIDs[i] >= 0) {
			sum += vertex.m_Weights[i];
		}
	}

	if (sum > 0.0f) {
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
			if (vertex.m_BoneIDs[i] >= 0) {
				vertex.m_Weights[i] /= sum;
			}
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	if (!mesh || !scene) {
		throw std::invalid_argument("Null mesh or scene passed to ExtractBoneWeightForVertices");
	}

	isAnimated = (mesh->mNumBones > 0);

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.name = boneName;

			aiNode* node = scene->mRootNode->FindNode(mesh->mBones[boneIndex]->mName);
			if (node && node->mParent) {
				newBoneInfo.parentName = node->mParent->mName.C_Str();
			}
			else {
				newBoneInfo.parentName = "";
			}

			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);

			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else {
			boneID = m_BoneInfoMap[boneName].id;
		}

		if (boneID < 0) {
			continue;
		}

		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;

			if (vertexId >= vertices.size()) {
				continue;
			}

			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}

	for (auto& vertex : vertices) {
		bool hasValidBone = false;
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
			if (vertex.m_BoneIDs[i] >= 0 && vertex.m_Weights[i] > 0.0f) {
				hasValidBone = true;
				break;
			}
		}

		if (!hasValidBone && isAnimated) {
			vertex.m_BoneIDs[0] = 0;
			vertex.m_Weights[0] = 1.0f;
		}
	}
}