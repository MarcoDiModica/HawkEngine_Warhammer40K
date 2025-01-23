#include "SkeletalAnimationComponent.h"
#include "SkeletalShaders.h"
//#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
//#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <glm/mat4x4.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>

SkeletalAnimationComponent::SkeletalAnimationComponent(GameObject* owner) : Component(owner) { name = "SkeletalAnimation"; }

void SkeletalAnimationComponent::Destroy() {
	// limpia aqui
}

std::shared_ptr<aiMesh> SkeletalAnimationComponent::GetMesh() const
{
	return std::shared_ptr<aiMesh>();
}

std::unique_ptr<Component> SkeletalAnimationComponent::Clone(GameObject* owner) {
	auto skeletalAnimation = std::make_unique<SkeletalAnimationComponent>(*this);
	skeletalAnimation->owner = owner;
	return skeletalAnimation;
}

inline glm::mat4 SkeletalAnimationComponent::assimpToGlmMatrix(aiMatrix4x4 matrix)
{
    glm::mat4 m;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			m[y][x] = matrix[x][y];
		}
	}
    return m;
}

inline glm::vec3 SkeletalAnimationComponent::assimpToGlmVec3(aiVector3D vec)
{

	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat SkeletalAnimationComponent::assimpToGlmQuat(aiQuaternion quat)
{
	return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

inline SDL_Window* initWindow(int& windowWidth, int& windowHeight) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_LoadLibrary(NULL);

	//window
	SDL_Window* window = SDL_CreateWindow("skin Animation",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);


	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


	//gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(1.0, 0.0, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);
	SDL_ShowWindow(window);
	SDL_GL_SetSwapInterval(1);
	return window;
}

unsigned int SkeletalAnimationComponent::createVertexArray(std::vector<Vertex>& vertices, std::vector<unsigned int> indices)
{
	unsigned int 
		vao = 0, 
		vbo = 0, 
		ebo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneIDs));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneWeights));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	return vao;
}

unsigned int SkeletalAnimationComponent::createTexture(std::string filepath)
{
	unsigned int textureId;
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 4);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

static std::pair<unsigned int, float> getTimeFraction(std::vector<float>& times, float& dt)
{
	unsigned int segment = 0;
	if (times.empty() || dt < times.front() || dt > times.back())
	{
		return { 0, 0.0f };
	}
	else 
	{
		while (dt >= times[segment])
		{
			segment++;
		}
		float start = times[segment - 1];
		float end = times[segment];
		float fraction = (dt - start) / (end - start);
		return { segment, fraction };
	}
}

inline unsigned int SkeletalAnimationComponent::createShader(const char* vertexStr, const char* fragmentStr) {
	int success;
	char info_log[512];
	unsigned int
		program = glCreateProgram(),
		vShader = glCreateShader(GL_VERTEX_SHADER),
		fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vertexStr, 0);
	glCompileShader(vShader);
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vShader, 512, 0, info_log);
		std::cout << "vertex shader compilation failed!\n" << info_log << std::endl;
	}
	glShaderSource(fShader, 1, &fragmentStr, 0);
	glCompileShader(fShader);
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fShader, 512, 0, info_log);
		std::cout << "fragment shader compilation failed!\n" << info_log << std::endl;
	}

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, 0, info_log);
		std::cout << "program linking failed!\n" << info_log << std::endl;
	}
	glDetachShader(program, vShader);
	glDeleteShader(vShader);
	glDetachShader(program, fShader);
	glDeleteShader(fShader);

	return program;
}

std::shared_ptr<Bone> SkeletalAnimationComponent::GetSkeleton() const
{
	return std::shared_ptr<Bone>();
}

std::shared_ptr<Animation> SkeletalAnimationComponent::GetAnimation() const
{
	return std::shared_ptr<Animation>();
}

bool SkeletalAnimationComponent::ReadSkeleton(Bone& boneOutput, aiNode* node, std::unordered_map<std::string, std::pair<int, glm::mat4>>& boneInfoTable)
{
	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
	{
		boneOutput.name = node->mName.C_Str();
		boneOutput.id = boneInfoTable[boneOutput.name].first;
		boneOutput.offset = boneInfoTable[boneOutput.name].second;

		for (int i = 0; i < node->mNumChildren; i++)
		{
			Bone child;
			ReadSkeleton(child, node->mChildren[i], boneInfoTable);
			boneOutput.children.push_back(child);
		}
		return true;
	}
	else
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			if (ReadSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
			{
				return true;
			}
		}
	}
	return false;
}

void SkeletalAnimationComponent::LoadModel(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& verticesOutput, std::vector<unsigned int>& indicesOutput, Bone& skeletonOutput, unsigned int& boneCount) {
	verticesOutput = {};
	indicesOutput = {};

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		glm::vec2 vec;
		vec.x = mesh->mTextureCoords[0][i].x;
		vec.y = mesh->mTextureCoords[0][i].y;
		vertex.uv = vec;

		vertex.boneIDs = glm::ivec4(0);
		vertex.boneWeights = glm::vec4(0.0f);

		verticesOutput.push_back(vertex);
	}


	std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo = {};
	std::vector<unsigned int> boneCounts;
	boneCounts.resize(verticesOutput.size(), 0);
	boneCount = mesh->mNumBones;

	for (unsigned int i = 0; i < boneCount; i++)
	{
		aiBone* bone = mesh->mBones[i];
		glm::mat4 m = assimpToGlmMatrix(bone->mOffsetMatrix);
		boneInfo[bone->mName.C_Str()] = { i,m };

		for (unsigned int j = 0; j < bone->mNumWeights; j++)
		{
			unsigned int id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;
			switch (boneCounts[id])
			{
			case 1:
				verticesOutput[id].boneIDs.x = i;
				verticesOutput[id].boneWeights.x = weight;
				break;
			case 2:
				verticesOutput[id].boneIDs.y = i;
				verticesOutput[id].boneWeights.y = weight;
				break;
			case 3:
				verticesOutput[id].boneIDs.z = i;
				verticesOutput[id].boneWeights.z = weight;
				break;
			case 4:
				verticesOutput[id].boneIDs.w = i;
				verticesOutput[id].boneWeights.w = weight;
				break;
			default:
				break;
			}
		}
	}

	for (int i = 0; i < verticesOutput.size(); i++)
	{
		glm::vec4& boneWeights = verticesOutput[i].boneWeights;
		float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
		if (totalWeight > 0.0f)
		{
			verticesOutput[i].boneWeights = glm::vec4(
				boneWeights.x / totalWeight,
				boneWeights.y / totalWeight,
				boneWeights.z / totalWeight,
				boneWeights.w / totalWeight
			);
		}
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indicesOutput.push_back(face.mIndices[j]);
	}

	ReadSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
}

void SkeletalAnimationComponent::LoadAnimation(const aiScene* scene, Animation* animation) {
	aiAnimation* anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0)
	{
		animation->ticksPerSecond = anim->mTicksPerSecond;
	}
	else
	{
		animation->ticksPerSecond = 1;
	}
	animation->duration = anim->mDuration * anim->mTicksPerSecond;
	animation->boneTransforms = {};

	for (int i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* channel = anim->mChannels[i];
		BoneTransformTrack track;
		for (int j = 0; j < channel->mNumPositionKeys; j++)
		{
			track.positionTimeStamps.push_back(channel->mPositionKeys[j].mTime);
			track.positions.push_back(assimpToGlmVec3(channel->mPositionKeys[j].mValue));
		}
		for (int j = 0; j < channel->mNumRotationKeys; j++)
		{
			track.rotationTimeStamps.push_back(channel->mRotationKeys[j].mTime);
			track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));
		}
		for (int j = 0; j < channel->mNumScalingKeys; j++)
		{
			track.scaleTimeStamps.push_back(channel->mScalingKeys[j].mTime);
			track.scales.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));
		}
		animation->boneTransforms[channel->mNodeName.C_Str()] = track;
	}
}

void SkeletalAnimationComponent::GetPose(Animation& animation, Bone& skeleton, float dt, std::vector<glm::mat4>& output, glm::mat4& parentTransform, glm::mat4& globalInverseTransform)
{
	BoneTransformTrack& btt = animation.boneTransforms[skeleton.name];
	dt = fmod(dt, animation.duration);
	std::pair<unsigned int, float> positionSegment = getTimeFraction(btt.positionTimeStamps, dt);

	glm::vec3 position1;
	if (!btt.positions.empty() && positionSegment.first > 0)
	{
		position1 = btt.positions[positionSegment.first - 1];
	}
	else
	{
		position1 = glm::vec3(0.0f);
	}
	glm::vec3 position2;
	if (positionSegment.first < btt.positions.size()) {
		position2 = btt.positions[positionSegment.first];
	}
	else {
		// Handle the error or assign a default value
		position2 = glm::vec3(0.0f);
	}
	glm::vec3 position = glm::mix(position1, position2, positionSegment.second);

	positionSegment = getTimeFraction(btt.rotationTimeStamps, dt);
	glm::quat rotation1;
	if (!btt.rotations.empty() && positionSegment.first > 0)
	{
		rotation1 = btt.rotations[positionSegment.first - 1];
	}
	else
	{
		rotation1 = glm::quat(1.0, 0.0, 0.0, 0.0);
	}
	glm::quat rotation2; 
	if (positionSegment.first < btt.rotations.size()) {
		rotation2 = btt.rotations[positionSegment.first];
	}
	else {
		// Handle the error or assign a default value
		rotation2 = glm::quat(1.0, 0.0, 0.0, 0.0);
	}
	glm::quat rotation = glm::slerp(rotation1, rotation2, positionSegment.second);

	positionSegment = getTimeFraction(btt.scaleTimeStamps, dt);
	glm::vec3 scale1;
	if (!btt.positions.empty() && positionSegment.first > 0)
	{
		scale1 = btt.scales[positionSegment.first - 1];
	}
	else
	{
		scale1 = glm::vec3(0.0f);
	}
	glm::vec3 scale2; 
	if (positionSegment.first < btt.positions.size()) {
		scale2 = btt.scales[positionSegment.first];
	}
	else {
		// Handle the error or assign a default value
		scale2 = glm::vec3(0.0f);
	}
	glm::vec3 scale = glm::mix(scale1, scale2, positionSegment.second);

	glm::mat4 positionMat = glm::mat4(1.0),
		scaleMat = glm::mat4(1.0);
	positionMat = glm::translate(positionMat, position);
	glm::mat4 rotationMat = glm::toMat4(rotation);
	scaleMat = glm::scale(scaleMat, scale);
	glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
	glm::mat4 globalTransform = parentTransform * localTransform;

	output[skeleton.id] = globalInverseTransform * globalTransform * skeleton.offset;

	for (Bone& child : skeleton.children)
	{
		GetPose(animation, child, dt, output, globalTransform, globalInverseTransform);
	}
}

void SkeletalAnimationComponent::Start()
{

}

void SkeletalAnimationComponent::Update(float deltaTime)
{
	//int windowWidth, windowHeight;
	//SDL_Window* window = initWindow(windowWidth, windowHeight);
	//bool isRunning = true;	

	//Assimp::Importer importer;
	//const char* filePath = "../MyAnimationEngine/AnimationSamples/Walking.fbx";
	//const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
	//if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	//{
	//	std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	//}
    //if (scene && scene->mMeshes && scene->mMeshes[0]) {
    //    meshes = scene->mMeshes[0];
    //} else {
    //    // Manejar el caso de error apropiadamente
    //    std::cerr << "Error: 'scene' o 'scene->mMeshes' es NULL." << std::endl;
    //}
	//glm::mat4 globalInverseTransform(1.0);
	//if (scene && scene->mRootNode) {
	//	globalInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	//}
	//else {
	//	// Manejar el caso de error apropiadamente
	//	std::cerr << "Error: 'scene' o 'scene->mRootNode' es NULL." << std::endl;
	//}
	////glm::mat4 globalInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	//globalInverseTransform = glm::inverse(globalInverseTransform);
	//
	//LoadModel(scene, meshes, vertices, indices, skeleton, boneCount);
	//LoadAnimation(scene, &animation);
	//
	//vao = createVertexArray(vertices, indices);
	//diffuseTexture = createTexture("../MyAnimationEngine/AnimationSamples/Skeleton_Model/Skeleton_D.png");
	//
	//glm::mat4 identity(1.0);
	//
	//std::vector<glm::mat4> currentPose = {};
	//currentPose.resize(boneCount, identity);
	//
	//unsigned int shader = createShader(vertexShaderSource, fragmentShaderSource);
	//
	//unsigned int viewProjectionMatrixLocation = glGetUniformLocation(shader, "viewProjectionMatrix");
	//unsigned int modelMatrixLocation = glGetUniformLocation(shader, "model_matrix");
	//unsigned int bonesMatricesLocation = glGetUniformLocation(shader, "bones_transforms");
	//unsigned int textureLocation = glGetUniformLocation(shader, "diff_texture");
    //
	//glm::mat4 projectionMatrix = glm::perspective(75.0f, 800.0f / 600.0f, 0.1f, 100.0f);
	//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 2.0f, -5.0f), 
	//	glm::vec3(0.0f, 0.0f, 0.0f), 
	//	glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	//
	//glm::mat4 modelMatrix(1.0f);
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
	//
	//while (isRunning) {
	//	SDL_Event ev;
	//	while (SDL_PollEvent(&ev)) {
	//		if (ev.type == SDL_QUIT)
	//			isRunning = false;
	//	}
	//
	//	float elapsedTime = (float)glfwGetTime() / 1000;
	//
	//	float dAngle = elapsedTime * 0.002;
	//	modelMatrix = glm::rotate(modelMatrix, dAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	//
	//	GetPose(animation, skeleton, elapsedTime, currentPose, identity, globalInverseTransform);
	//
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glUseProgram(shader);
	//	glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
	//	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	//	glUniformMatrix4fv(bonesMatricesLocation, boneCount, GL_FALSE, glm::value_ptr(currentPose[0]));
	//
	//	glBindVertexArray(vao);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	//	glUniform1i(textureLocation, 0);
	//
	//	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//
	//	SDL_GL_SwapWindow(window);
	//}
	////cleanup
	//SDL_GLContext context = SDL_GL_GetCurrentContext();
	//SDL_GL_DeleteContext(context);
	//SDL_DestroyWindow(window);
	//SDL_Quit();
	
}

