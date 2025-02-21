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

void ModelImporter::graphicObjectFromNode(const aiScene& scene, const aiNode& node, const vector<shared_ptr<Mesh>>& meshes, const vector<shared_ptr<Material>>& materials, const string& scenePath, glm::mat4 accumulatedTransform = glm::mat4(1.0f)) {
	GameObject obj;
	if (hasSubstring(node.mName.data, "$AssimpFbx$")) {

		// Accumulate the transformation.
		glm::mat4 nodeTransform = aiMat4ToMat4(node.mTransformation);

		
		accumulatedTransform = accumulatedTransform * nodeTransform;

		// Recurse for the child nodes, passing the accumulated transformation.
		for (unsigned int i = 0; i < node.mNumChildren; ++i) {
			graphicObjectFromNode(scene, *node.mChildren[i], meshes, materials, scenePath, accumulatedTransform);
		}
	}

	if (!hasSubstring(node.mName.data, "$AssimpFbx$")) {
		glm::mat4 localMatrix = aiMat4ToMat4(node.mTransformation);
		
		obj.GetTransform()->SetMatrix(localMatrix * accumulatedTransform);
		obj.SetName(node.mName.data);
		for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
			const auto meshIndex = node.mMeshes[i];
			const auto materialIndex = scene.mMeshes[meshIndex]->mMaterialIndex;

			//auto& child = i == 0 ? obj : obj.emplaceChild();

			auto meshComponent = obj.AddComponent<MeshRenderer>();
			meshComponent->SetMesh(meshes[meshIndex]);
			meshComponent->SetMaterial(materials[materialIndex]);

			if (scene.mNumAnimations > 0 ) 
			{
				auto skeletalAnimationComponent = obj.AddComponent<SkeletalAnimationComponent>();
				Animation* animation = new Animation();
				Animation animation2 = Animation();
				animation->SetUpAnimation(scenePath, meshComponent->GetMesh().get());
				animation2 = *animation;
				obj.GetComponent<SkeletalAnimationComponent>()->animationTest = animation2;
				obj.GetComponent<SkeletalAnimationComponent>()->SetAnimation(animation);
				obj.GetComponent<SkeletalAnimationComponent>()->patatudo = 43.0f;
				skeletalAnimationComponent->animationTest = animation2;
				animations.push_back(std::make_shared<Animation>(*animation));
				skeletalAnimationComponent->SetAnimation(animation);
				std::cout << node.mName.C_Str() << std::endl;
			}

			//meshComponent->GetMaterial()->loadShaders("Assets/Shaders/vertex_shader.glsl", "Assets/Shaders/fragment_shader.glsl");
			meshGameObjects.push_back(std::make_shared<GameObject>(obj));
		}

		for (unsigned int i = 0; i < node.mNumChildren; ++i)
		{

			graphicObjectFromNode(scene, *node.mChildren[i], meshes, materials, scenePath, accumulatedTransform);

		}
	}


}

vector<shared_ptr<Mesh>> createMeshesFromFBX(const aiScene& scene) {
	vector<shared_ptr<Mesh>> meshess;
	if (!scene.mMeshes) {
		// Handle error: scene.mMeshes is null
		return meshess;
	}
	for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
		if (!scene.mMeshes[i]) {
			// Handle error: scene.mMeshes[i] is null
			continue;
		}
		aiMesh* fbx_mesh = scene.mMeshes[i];
		auto mesh_ptr = make_shared<Mesh>();

		vector<unsigned int> indices(fbx_mesh->mNumFaces * 3);
		for (unsigned int j = 0; j < fbx_mesh->mNumFaces; ++j) {
			indices[j * 3 + 0] = fbx_mesh->mFaces[j].mIndices[0];
			indices[j * 3 + 1] = fbx_mesh->mFaces[j].mIndices[1];
			indices[j * 3 + 2] = fbx_mesh->mFaces[j].mIndices[2];
		}

		mesh_ptr->Load(reinterpret_cast<const glm::vec3*>(fbx_mesh->mVertices), fbx_mesh->mNumVertices, indices.data(), indices.size());
		if (fbx_mesh->HasTextureCoords(0)) {
			vector<glm::vec2> texCoords(fbx_mesh->mNumVertices);
			for (unsigned int j = 0; j < fbx_mesh->mNumVertices; ++j) texCoords[j] = glm::vec2(fbx_mesh->mTextureCoords[0][j].x, fbx_mesh->mTextureCoords[0][j].y);
			mesh_ptr->loadTexCoords(texCoords.data(), texCoords.size());
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glEnableVertexAttribArray(1);
		}
		if (fbx_mesh->HasNormals()) mesh_ptr->LoadNormals(reinterpret_cast<glm::vec3*>(fbx_mesh->mNormals), fbx_mesh->mNumVertices);
		if (fbx_mesh->HasVertexColors(0)) {
			vector<glm::u8vec3> colors(fbx_mesh->mNumVertices);
			for (unsigned int j = 0; j < fbx_mesh->mNumVertices; ++j) colors[j] = glm::u8vec3(fbx_mesh->mColors[0][j].r, fbx_mesh->mColors[0][j].g, fbx_mesh->mColors[0][j].b);
			mesh_ptr->LoadColors(colors.data(), colors.size());
		}
		
		vector<Vertex> vertices;
		for (unsigned int j = 0; j < fbx_mesh->mNumVertices; ++j) 
		{
			Vertex vertex;

			mesh_ptr->SetVertexBoneDataToDefault(vertex);
			vertices.push_back(vertex);
		}


        mesh_ptr->ExtractBoneWeightForVertices(vertices, fbx_mesh, &scene);

		mesh_ptr->LoadBones();


		meshess.push_back(mesh_ptr);
	}
	return meshess;
}




vector<shared_ptr<Material>> createMaterialsFromFBX(const aiScene& scene, const fs::path& basePath) {

	vector<shared_ptr<Material>> materials;
	map<string, shared_ptr<Image>> images;

	for (unsigned int i = 0; i < scene.mNumMaterials; ++i) {
		const auto* fbx_material = scene.mMaterials[i];
		auto material = make_shared<Material>();

		if (fbx_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			const string textureFileName = fs::path(texturePath.C_Str()).filename().string();
			const auto image_itr = images.find(textureFileName);
			if (image_itr != images.end()) material->setImage(image_itr->second);
			else {
				shared_ptr<Image> image = std::make_shared<Image>();
				//images.insert({ textureFileName, LoadTexture((basePath / textureFileName).string()) });
				image->LoadTexture((basePath / textureFileName).string());
				material->setImage(image);
			}

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
		else
		{
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			const string textureFileName = fs::path(texturePath.C_Str()).filename().string();
			const auto image_itr = images.find(textureFileName);
			if (image_itr != images.end()) material->setImage(image_itr->second);
			else {
				shared_ptr<Image> image = std::make_shared<Image>();
				//images.insert({ textureFileName, LoadTexture((basePath / textureFileName).string()) });
				image->LoadTexture((basePath / textureFileName).string());
				material->setImage(image);
			}

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

		aiColor4D color;
		fbx_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->color = vec4(color.r , color.g , color.b , color.a );

		LOG(LogType::LOG_ASSIMP, "color %f %f %f %f ", color.r, color.g, color.b, color.a);

		if (material == nullptr) {

			materials;
		}

		if (material->image().id() != NULL) {

		}

		materials.push_back(material);
	}
	return materials;
}

void ModelImporter::loadFromFile(const std::string& path) {
	const aiScene* fbx_scene = aiImportFile(path.c_str(), aiProcess_CalcTangentSpace |aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices | 
		aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_FlipUVs );
	aiGetErrorString();
	meshes = createMeshesFromFBX(*fbx_scene);

	// must giv name to the meshes

	for (unsigned int i = 0; i < fbx_scene->mNumMeshes; ++i) {

		const aiMesh* mesh = fbx_scene->mMeshes[i];

		meshes[i]->nameM = std::string(mesh->mName.C_Str());

	}

	EncodeFBXScene(path, meshes, fbx_scene);

	const auto materials = createMaterialsFromFBX(*fbx_scene, "Assets/Textures/"/*fs::absolute(path).parent_path()*/);

	/*GameObject fbx_obj =*/ graphicObjectFromNode(*fbx_scene, *fbx_scene->mRootNode, meshes, materials, path, glm::mat4(1.0f));
	aiReleaseImport(fbx_scene);
	//return std::make_shared<GameObject>(fbx_obj);
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
