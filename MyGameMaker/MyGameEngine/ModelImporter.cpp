#define _CRT_SECURE_NO_WARNINGS
#include "ModelImporter.h"
#include <map>
#include <filesystem>
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
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
	// Extraer la traslación
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

	// Extraer la rotación
	rotation = glm::quat(rotationMatrix);
}

bool hasSubstring(const std::string& str, const std::string& substr) {
	return str.find(substr) != std::string::npos;
}

glm::mat4 accumulatedTransform;
std::shared_ptr<GameObject> rootObject;

void ModelImporter::graphicObjectFromNode(const aiScene& scene, const aiNode& node, const vector<shared_ptr<Mesh>>& meshes, const vector<shared_ptr<Material>>& materials, glm::mat4 accumulatedTransform) {
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

			// Coordenadas de los vértices
			aiVector3D vertex = mesh->mVertices[j];
			vec3 aux = vec3(vertex.x, vertex.y, vertex.z);
			Vertex v;
			v.position = aux;

			// Coordenadas UV (si existen)
			if (mesh->mTextureCoords[0]) {  // Comprueba si hay UVs
				aiVector3D uv = mesh->mTextureCoords[0][j];
				v.texCoords = vec2(uv.x, uv.y);
				aux.x = uv.x;  // Solo X y Y
				aux.y = uv.y;
			}

			// Normales (si existen)
			if (mesh->HasNormals()) {  // Verifica si hay normales
				aiVector3D normal = mesh->mNormals[j];
				v.normal = vec3(normal.x, normal.y, normal.z);
			}

			// Tangentes y bitangentes (para normal mapping)
			if (mesh->HasTangentsAndBitangents()) {
				aiVector3D tangent = mesh->mTangents[j];
				aiVector3D bitangent = mesh->mBitangents[j];
				v.tangent = vec3(tangent.x, tangent.y, tangent.z);
				v.bitangent = vec3(bitangent.x, bitangent.y, bitangent.z);
			}

			modelsData[i]->vertexData.push_back(v);
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

// Check if a texture path exists and return a valid path or empty string
std::string findTexturePath(const aiString& texturePath, const fs::path& basePath) {
	// Try to find texture with original path
	if (fs::exists(basePath / texturePath.C_Str())) {
		return (basePath / texturePath.C_Str()).string();
	}

	// Try with just the filename
	std::string filename = std::filesystem::path(texturePath.C_Str()).filename().string();
	if (fs::exists(basePath / filename)) {
		return (basePath / filename).string();
	}

	LOG(LogType::LOG_WARNING, "Texture not found: %s", texturePath.C_Str());
	return "";
}

std::vector<std::shared_ptr<Material>>createMaterialsFromFBX(const aiScene& scene, const fs::path& basePath) {
	std::vector<std::shared_ptr<Material>> materials;
	materials.resize(scene.mNumMaterials);

	for (unsigned int i = 0; i < scene.mNumMaterials; ++i) {
		const auto* fbx_material = scene.mMaterials[i];
		materials[i] = std::make_shared<Material>();

		// Set shader type to PBR by default
		materials[i]->SetShaderType(PBR);

		// Extract albedo/diffuse texture and color
		if (fbx_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

			std::string fullPath = findTexturePath(texturePath, basePath);
			if (!fullPath.empty()) {
				auto albedoMap = std::make_shared<Image>();
				if (albedoMap->LoadTexture(fullPath)) {
					materials[i]->SetAlbedoMap(albedoMap);
				}
			}
		}

		// Extract normal map if available
		if (fbx_material->GetTextureCount(aiTextureType_NORMALS) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_NORMALS, 0, &texturePath);

			std::string fullPath = findTexturePath(texturePath, basePath);
			if (!fullPath.empty()) {
				auto normalMap = std::make_shared<Image>();
				if (normalMap->LoadTexture(fullPath)) {
					materials[i]->SetNormalMap(normalMap);
				}
			}
		}

		// Extract roughness map if available (might be in SHININESS or METALNESS)
		if (fbx_material->GetTextureCount(aiTextureType_SHININESS) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_SHININESS, 0, &texturePath);

			std::string fullPath = findTexturePath(texturePath, basePath);
			if (!fullPath.empty()) {
				auto roughnessMap = std::make_shared<Image>();
				if (roughnessMap->LoadTexture(fullPath)) {
					materials[i]->SetRoughnessMap(roughnessMap);
				}
			}
		}

		// Extract metalness map if available
		if (fbx_material->GetTextureCount(aiTextureType_METALNESS) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_METALNESS, 0, &texturePath);

			std::string fullPath = findTexturePath(texturePath, basePath);
			if (!fullPath.empty()) {
				auto metalnessMap = std::make_shared<Image>();
				if (metalnessMap->LoadTexture(fullPath)) {
					materials[i]->SetMetalnessMap(metalnessMap);
				}
			}
		}

		// Extract ambient occlusion map if available
		if (fbx_material->GetTextureCount(aiTextureType_AMBIENT) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);

			std::string fullPath = findTexturePath(texturePath, basePath);
			if (!fullPath.empty()) {
				auto aoMap = std::make_shared<Image>();
				if (aoMap->LoadTexture(fullPath)) {
					materials[i]->SetAOMap(aoMap);
				}
			}
		}

		// Extract emissive map if available
		if (fbx_material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath);

			std::string fullPath = findTexturePath(texturePath, basePath);
			if (!fullPath.empty()) {
				auto emissiveMap = std::make_shared<Image>();
				if (emissiveMap->LoadTexture(fullPath)) {
					materials[i]->SetEmissiveMap(emissiveMap);
				}
			}
		}

		// Extract material properties
		aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
		if (fbx_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
			materials[i]->SetColor(vec4(color.r, color.g, color.b, color.a));
		}

		// Extract PBR values if available
		float roughnessValue = 0.5f;
		if (fbx_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessValue) == AI_SUCCESS) {
			materials[i]->SetRoughness(roughnessValue);
		}

		float metalnessValue = 0.0f;
		if (fbx_material->Get(AI_MATKEY_METALLIC_FACTOR, metalnessValue) == AI_SUCCESS) {
			materials[i]->SetMetalness(metalnessValue);
		}

		// Extract emissive properties
		aiColor3D emissiveColor(0.0f, 0.0f, 0.0f);
		if (fbx_material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == AI_SUCCESS) {
			materials[i]->SetEmissiveColor(vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b));

			// If there's emissive color, set a default intensity
			if (emissiveColor.r > 0.0f || emissiveColor.g > 0.0f || emissiveColor.b > 0.0f) {
				materials[i]->SetEmissiveIntensity(1.0f);
			}
		}
	}

	return materials;
}

void ModelImporter::loadFromFile(const std::string& path) {
	const aiScene* fbx_scene = aiImportFile(path.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		aiProcess_FlipUVs);

	if (!fbx_scene) {
		LOG(LogType::LOG_ERROR, "Failed to load model: %s", path.c_str());
		LOG(LogType::LOG_ERROR, "Assimp error: %s", aiGetErrorString());
		return;
	}

	// Create models and materials from FBX
	meshes = createMeshesFromFBX(*fbx_scene);

	materials = createMaterialsFromFBX(*fbx_scene, "Assets/Textures/");

	// Create the GameObject hierarchy
	graphicObjectFromNode(*fbx_scene, *fbx_scene->mRootNode, meshes, materials, glm::mat4(1.0f));

	// Link meshes with materials in the fbx objects
	for (auto& obj : fbx_object) {
		auto meshRenderer = obj->AddComponent<MeshRenderer>();

		// Find the mesh for this object
		for (unsigned int i = 0; i < fbx_scene->mRootNode->mNumMeshes; ++i) {
			auto meshIndex = fbx_scene->mRootNode->mMeshes[i];
			if (meshIndex < meshes.size()) {
				meshRenderer->SetMesh(meshes[meshIndex]);

				// Find the corresponding material
				auto materialIndex = fbx_scene->mMeshes[meshIndex]->mMaterialIndex;
				if (materialIndex < materials.size()) {
					meshRenderer->SetMaterial(materials[materialIndex]);

					// Add shader component
					auto shaderComp = obj->AddComponent<ShaderComponent>();
					shaderComp->SetShaderType(materials[materialIndex]->GetShaderType());
					shaderComp->SetOwnerMaterial(materials[materialIndex].get());
				}
				break;
			}
		}
	}

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