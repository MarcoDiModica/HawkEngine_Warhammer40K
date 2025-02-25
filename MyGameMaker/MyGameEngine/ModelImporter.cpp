#define _CRT_SECURE_NO_WARNINGS
#include "ModelImporter.h"
#include <map>
#include <filesystem>
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include "MyAnimationEngine/SkeletalAnimationComponent.h"
#include "MyAnimationEngine/Animation.h"

#include "Material.h"
#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "Image.h"
#include "Model.h"
#include "../MyShadersEngine/ShaderComponent.h"
#include "../MyGameEditor/App.h"
#include "SceneManager.h"

//#include "ImageImporter.h"
#include "GameObject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/matrix_decompose.hpp"
// cosa ilegal
#include "../MyGameEditor/Log.h"

using namespace std;
namespace fs = std::filesystem;

static mat4 aiMat4ToMat4(const aiMatrix4x4& aiMat) {
	mat4 mat;
	for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r) mat[c][r] = aiMat[r][c];
	return mat;
}

static void decomposeMatrix(const mat4& matrix, vec3& scale, glm::quat& rotation, vec3& translation) {
	// Extraer la traslaci�n
	translation = vec3(matrix[3]);

	// Extraer la escala
	scale = vec3(
		length(vec3(matrix[0])),
		length(vec3(matrix[1])),
		length(vec3(matrix[2]))
	);

	// Remover la escala de la matriz
	mat4 rotationMatrix = matrix;
	rotationMatrix[0] /= scale.x;
	rotationMatrix[1] /= scale.y;
	rotationMatrix[2] /= scale.z;

	// Extraer la rotaci�n
	rotation = glm::quat(rotationMatrix);
}

bool hasSubstring(const std::string& str, const std::string& substr) {
	return str.find(substr) != std::string::npos;
}

glm::mat4 accumulatedTransform;
std::shared_ptr<GameObject> rootObject;

void ModelImporter::graphicObjectFromNode(const aiScene& scene, const aiNode& node, const vector<shared_ptr<Mesh>>& meshes, const vector<shared_ptr<Material>>& materials, glm::mat4 accumulatedTransform = glm::mat4(1.0f)) {
	GameObject obj;
	// Check if the node has the suffix indicating it is an Assimp transformation node
	if (hasSubstring(node.mName.data, "$AssimpFbx$")) {

		// Accumulate the transformation.
		glm::mat4 nodeTransform = aiMat4ToMat4(node.mTransformation);
		accumulatedTransform = accumulatedTransform * nodeTransform;

		// Recurse for the child nodes, passing the accumulated transformation.
		for (unsigned int i = 0; i < node.mNumChildren; ++i) {
			graphicObjectFromNode(scene, *node.mChildren[i], meshes, materials, accumulatedTransform);
		}
	}

	// If the current node is not a transformation node, create a GameObject
	if (!hasSubstring(node.mName.data, "$AssimpFbx$")) {
		glm::mat4 localMatrix = aiMat4ToMat4(node.mTransformation);

		obj.GetTransform()->SetMatrix(localMatrix * accumulatedTransform);
		
		// Add mesh and material components
		for (unsigned int i = 0; i < node.mNumMeshes; ++i) {

			const auto meshIndex = node.mMeshes[i];
			const auto& model = meshes[meshIndex];


			if (scene.mNumAnimations > 0)
			{
				Animation* animation = new Animation();

				animation->SetUpAnimation(scenePath, model->getModel().get());
		
				animations.push_back(std::shared_ptr<Animation>(animation));
				std::cout << node.mName.C_Str() << std::endl;
			}

			obj.SetName(node.mName.data);

			fbx_object.push_back(std::make_shared<GameObject>(obj));
		}

		// Process and add children as children of the current node
		for (unsigned int i = 0; i < node.mNumChildren; ++i) {
			graphicObjectFromNode(scene, *node.mChildren[i], meshes, materials, accumulatedTransform);
		}
	}
}

std::vector<std::shared_ptr<Mesh>>createMeshesFromFBX(const aiScene& scene) {
	std::vector<std::shared_ptr<Mesh>> meshes;
	meshes.resize(scene.mNumMeshes);

	std::vector<std::shared_ptr<Model>> models;
	models.resize(scene.mNumMeshes);

	std::vector<std::shared_ptr<ModelData>> modelsData;
	modelsData.resize(scene.mNumMeshes);

	for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
		aiMesh* mesh = scene.mMeshes[i];
		modelsData[i] = std::make_shared<ModelData>();
		models[i] = std::make_shared<Model>();
		models[i]->SetMeshName(mesh->mName.C_Str());
		models[i]->SetMaterialIndex(mesh->mMaterialIndex);

		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {

			// Coordenadas de los v�rtices
			aiVector3D vertex = mesh->mVertices[j];
			vec3 aux = vec3(vertex.x, vertex.y, vertex.z);
			Vertex v;
			models[i]->SetVertexBoneDataToDefault(v);
			v.position = AssimpGLMHelpers::GetGLMVec(vertex);
			modelsData[i]->vertexData.push_back(v);
			

			// Coordenadas UV (si existen)
			if (mesh->mTextureCoords[0]) {  // Comprueba si hay UVs
				aiVector3D uv = mesh->mTextureCoords[0][j];
				aux.x = uv.x;  // Solo X y Y
				aux.y = uv.y;
			}
			modelsData[i]->vertex_texCoords.push_back(aux);

			if (mesh->HasNormals()) {  // Verifica si hay normales
				aiVector3D normal = mesh->mNormals[j];
				vec3 auxNormal(normal.x, normal.y, normal.z);
				modelsData[i]->vertex_normals.push_back(auxNormal);
			}

			if (mesh->HasVertexColors(0)) {  // Verifica si hay colores
				aiColor4D color = mesh->mColors[0][j];
				vec3 auxColor(color.r, color.g, color.b);
				modelsData[i]->vertex_colors.push_back(auxColor);
			}
		}

		models[i]->ExtractBoneWeightForVertices(modelsData[i]->vertexData, mesh, &scene);

		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++) {
				modelsData[i]->indexData.push_back(face.mIndices[k]);
			}
		}

		models[i]->SetModelData(*modelsData[i]);
	}

	for (int i = 0; i < models.size(); i++) {
		meshes[i] = std::make_shared<Mesh>();
		meshes[i]->setModel(models[i]);
	}

	return meshes;
}

std::vector<std::shared_ptr<Material>>createMaterialsFromFBX(const aiScene& scene, const fs::path& basePath) {
	std::vector<std::shared_ptr<Material>> materials;
	materials.resize(scene.mNumMaterials);

	for (unsigned int i = 0; i < scene.mNumMaterials; ++i) {
		const auto* fbx_material = scene.mMaterials[i];
		materials[i] = std::make_shared<Material>();
		materials[i]->imagePtr = std::make_shared<Image>();

		if (fbx_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			const string textureFileName = std::filesystem::path(texturePath.C_Str()).filename().string();
			materials[i]->imagePtr->image_path = (basePath / textureFileName).string();
			materials[i]->imagePtr->LoadTexture((basePath / textureFileName).string());

			auto uWrapMode = aiTextureMapMode_Wrap;
			auto vWrapMode = aiTextureMapMode_Wrap;
			fbx_material->Get(AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), uWrapMode);
			fbx_material->Get(AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), vWrapMode);
			assert(uWrapMode == aiTextureMapMode_Wrap);
			assert(vWrapMode == aiTextureMapMode_Wrap);

			unsigned int flags = 0;
			fbx_material->Get(AI_MATKEY_TEXFLAGS_DIFFUSE(0), flags);
			assert(flags == 0);
		}
		else {
			materials[i]->imagePtr->image_path = "";
		}

		aiColor4D color;
		fbx_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		materials[i]->color = vec4(color.r, color.g, color.b, color.a);

	}

	return materials;
}

void ModelImporter::loadFromFile(const std::string& path) {
	const aiScene* fbx_scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_SortByPType |
		aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_FlipUVs );
	aiGetErrorString();
	
	// Create models and materials from FBX
	meshes = createMeshesFromFBX(*fbx_scene);

	materials = createMaterialsFromFBX(*fbx_scene, "Assets/Textures/");

	scenePath = path;

	// Create the GameObject hierarchy
	graphicObjectFromNode(*fbx_scene, *fbx_scene->mRootNode, meshes, materials);
	aiReleaseImport(fbx_scene);

}

void ModelImporter::EncodeFBXScene(const std::string path, std::vector<std::shared_ptr<Mesh>> meshes, const aiScene* fbx_scene) {

	std::string name = path;
	name.erase(0, 14);
	std::string fullPath = "Library/Mesh/" + name + ".mesh";

	try {
		if (!std::filesystem::exists("Library/Mesh")) {
			if (!std::filesystem::create_directory("Library/Mesh")) {
				throw std::runtime_error("Error creating Library/Mesh");
			}
		}

		std::ofstream fout(fullPath, std::ios::binary);
		if (!fout.is_open()) {
			throw std::runtime_error("Error opening " + fullPath);
		}

		for (int i = 0; i < meshes.size(); ++i) {
			meshes[i]->SaveBinary(fbx_scene->mMeshes[i]->mName.C_Str());
		}

		fout.close();

	}
	catch (const std::exception& e) {
		LOG(LogType::LOG_ERROR, "Error saving scene FBX: %s", e.what());
	}
}
