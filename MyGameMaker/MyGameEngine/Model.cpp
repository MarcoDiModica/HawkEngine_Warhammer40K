#include "Model.h"
#include "AssimpGLMHelpers.h"

std::unordered_map<uint32_t, std::weak_ptr<Model>> ModelRegistry::s_Models;

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.name = boneName;

			// Quiero conseguir el nombre del parent
			aiNode* node = scene->mRootNode->FindNode(mesh->mBones[boneIndex]->mName);
			aiNode* parent = node->mParent;
			if (parent != nullptr)
			{
				newBoneInfo.parentName = parent->mName.C_Str();
			}
			else
			{
				newBoneInfo.parentName = "";
			}
			
			
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

void ModelRegistry::RegisterModel(std::shared_ptr<Model> model)
{
	if (model)
		s_Models[model->GetID()] = model;
}

void ModelRegistry::UnregisterModel(uint32_t id)
{
	s_Models.erase(id);
}

std::shared_ptr<Model> ModelRegistry::GetModelByID(uint32_t id)
{
	auto it = s_Models.find(id);
	if (it != s_Models.end() && !it->second.expired())
		return it->second.lock();
	return nullptr;
}