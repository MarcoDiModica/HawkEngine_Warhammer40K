#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "Mesh.h"
#include "Material.h"
#include "Image.h"
#include "types.h"
#include <iostream>


std::shared_ptr<GameObject> GameObject::CreateEmptyGameObject(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	//push back to the current scene
	return go;

}

std::shared_ptr<GameObject> GameObject::CreateCube(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	auto meshRenderer = go->AddComponent<MeshRenderer>();
	auto mesh = std::make_shared<Mesh>();

	const glm::vec3 vertices[] = {
		vec3(-0.5f, -0.5f, 0.5f),
		vec3(0.5f, -0.5f, 0.5f),
		vec3(0.5f, 0.5f, 0.5f),
		vec3(-0.5f, 0.5f, 0.5f),
		vec3(-0.5f, -0.5f, -0.5f),
		vec3(0.5f, -0.5f, -0.5f),
		vec3(0.5f, 0.5f, -0.5f),
		vec3(-0.5f, 0.5f, -0.5f)
	};

	const unsigned int indices[] = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		7, 6, 5, 5, 4, 7,
		4, 0, 3, 3, 7, 4,
		3, 2, 6, 6, 7, 3,
		4, 5, 1, 1, 0, 4
	};

	const glm::vec2 texCoords[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	const glm::vec3 normals[] = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f)
	};

	const glm::u8vec3 colors[] = {
		u8vec3(255, 0, 0),
		u8vec3(0, 255, 0),
		u8vec3(0, 0, 255),
		u8vec3(255, 255, 0),
		u8vec3(255, 0, 255),
		u8vec3(0, 255, 255)
	};

	mesh->Load(vertices, 8, indices, 36);
	mesh->loadTexCoords(texCoords, 4);
	mesh->LoadNormals(normals, 6);
	mesh->LoadColors(colors, 6);

	meshRenderer->SetMesh(mesh);


	//scene.emplaceChild(go);
	return go;
}

std::shared_ptr<GameObject> GameObject::CreateSphere(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	auto meshRenderer = go->AddComponent<MeshRenderer>();
	auto mesh = std::make_shared<Mesh>();

	const int stacks = 20;
	const int slices = 20;
	const float radius = 0.5f;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i <= stacks; i++)
	{
		float V = i / (float)stacks;
		float phi = V * glm::pi<float>();

		for (int j = 0; j <= slices; j++)
		{
			float U = j / (float)slices;
			float theta = U * (glm::two_pi<float>());

			float x = cos(theta) * sin(phi);
			float y = cos(phi);
			float z = sin(theta) * sin(phi);

			vertices.push_back(glm::vec3(x, y, z) * radius);
		}
	}

	for (int i = 0; i < slices * stacks + slices; i++)
	{
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);

		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	const glm::vec2 texCoords[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	const glm::vec3 normals[] = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f)
	};

	const glm::u8vec3 colors[] = {
		u8vec3(255, 0, 0),
		u8vec3(0, 255, 0),
		u8vec3(0, 0, 255),
		u8vec3(255, 255, 0),
		u8vec3(255, 0, 255),
		u8vec3(0, 255, 255)
	};

	mesh->Load(vertices.data(), vertices.size(), indices.data(), indices.size());
	mesh->loadTexCoords(texCoords, 4);
	mesh->LoadNormals(normals, 6);
	mesh->LoadColors(colors, 6);

	meshRenderer->SetMesh(mesh);

	//scene.emplaceChild(go);
	return go;
}

std::shared_ptr<GameObject> GameObject::CreatePlane(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	auto meshRenderer = go->AddComponent<MeshRenderer>();
	auto mesh = std::make_shared<Mesh>();

	const glm::vec3 vertices[] = {
		vec3(-0.5f, 0.0f, 0.5f),
		vec3(0.5f, 0.0f, 0.5f),
		vec3(0.5f, 0.0f, -0.5f),
		vec3(-0.5f, 0.0f, -0.5f)
	};

	const unsigned int indices[] = {
		0, 1, 2, 2, 3, 0
	};

	const glm::vec2 texCoords[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	const glm::vec3 normals[] = {
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	};

	const glm::u8vec3 colors[] = {
		u8vec3(255, 0, 0),
		u8vec3(0, 255, 0),
		u8vec3(0, 0, 255),
		u8vec3(255, 255, 0),
		u8vec3(255, 0, 255),
		u8vec3(0, 255, 255)
	};

	mesh->Load(vertices, 4, indices, 6);
	mesh->loadTexCoords(texCoords, 4);
	mesh->LoadNormals(normals, 4);
	mesh->LoadColors(colors, 4);

	meshRenderer->SetMesh(mesh);

	//scene.emplaceChild(go);
	return go;
}

std::shared_ptr<GameObject> GameObject::CreateQuad(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	auto meshRenderer = go->AddComponent<MeshRenderer>();
	auto mesh = std::make_shared<Mesh>();

	const glm::vec3 vertices[] = {
		vec3(-0.5f, -0.5f, 0.0f),
		vec3(0.5f, -0.5f, 0.0f),
		vec3(0.5f, 0.5f, 0.0f),
		vec3(-0.5f, 0.5f, 0.0f)
	};

	const unsigned int indices[] = {
		0, 1, 2, 2, 3, 0
	};

	const glm::vec2 texCoords[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	const glm::vec3 normals[] = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f)
	};

	const glm::u8vec3 colors[] = {
		u8vec3(255, 0, 0),
		u8vec3(0, 255, 0),
		u8vec3(0, 0, 255),
		u8vec3(255, 255, 0),
		u8vec3(255, 0, 255),
		u8vec3(0, 255, 255)
	};

	mesh->Load(vertices, 4, indices, 6);
	mesh->loadTexCoords(texCoords, 4);
	mesh->LoadNormals(normals, 4);
	mesh->LoadColors(colors, 4);

	meshRenderer->SetMesh(mesh);

	//scene.emplaceChild(go);
	return go;
}

std::shared_ptr<GameObject> GameObject::CreateCylinder(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	auto meshRenderer = go->AddComponent<MeshRenderer>();
	auto mesh = std::make_shared<Mesh>();

	const int stacks = 20;
	const int slices = 20;
	const float radius = 0.5f;
	const float height = 1.0f;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i <= stacks; i++)
	{
		float V = i / (float)stacks;
		float phi = V * glm::two_pi<float>();

		for (int j = 0; j <= slices; j++)
		{
			float U = j / (float)slices;
			float theta = U * (glm::two_pi<float>());

			float x = cos(theta) * radius;
			float y = height / 2 - V * height;
			float z = sin(theta) * radius;

			vertices.push_back(glm::vec3(x, y, z));
		}
	}

	for (int i = 0; i < slices * stacks + slices; i++)
	{
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);

		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	const glm::vec2 texCoords[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	const glm::vec3 normals[] = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f)
	};

	const glm::u8vec3 colors[] = {
		u8vec3(255, 0, 0),
		u8vec3(0, 255, 0),
		u8vec3(0, 0, 255),
		u8vec3(255, 255, 0),
		u8vec3(255, 0, 255),
		u8vec3(0, 255, 255)
	};

	mesh->Load(vertices.data(), vertices.size(), indices.data(), indices.size());
	mesh->loadTexCoords(texCoords, 4);
	mesh->LoadNormals(normals, 6);
	mesh->LoadColors(colors, 6);

	meshRenderer->SetMesh(mesh);

	//scene.emplaceChild(go);
	return go;
}

std::shared_ptr<GameObject> GameObject::CreateCapsule(const std::string& name)
{
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name);
	auto meshRenderer = go->AddComponent<MeshRenderer>();
	auto mesh = std::make_shared<Mesh>();

	const int stacks = 20;
	const int slices = 20;
	const float radius = 0.5f;
	const float height = 1.0f;

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i <= stacks; i++)
	{
		float V = i / (float)stacks;
		float phi = V * glm::pi<float>();

		for (int j = 0; j <= slices; j++)
		{
			float U = j / (float)slices;
			float theta = U * (glm::two_pi<float>());

			float x = cos(theta) * radius;
			float y = height / 2 - V * height;
			float z = sin(theta) * radius;

			vertices.push_back(glm::vec3(x, y, z));
		}
	}

	for (int i = 0; i < slices * stacks + slices; i++)
	{
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);

		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	const glm::vec2 texCoords[] = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	const glm::vec3 normals[] = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f)
	};

	const glm::u8vec3 colors[] = {
		u8vec3(255, 0, 0),
		u8vec3(0, 255, 0),
		u8vec3(0, 0, 255),
		u8vec3(255, 255, 0),
		u8vec3(255, 0, 255),
		u8vec3(0, 255, 255)
	};

	mesh->Load(vertices.data(), vertices.size(), indices.data(), indices.size());
	mesh->loadTexCoords(texCoords, 4);
	mesh->LoadNormals(normals, 6);
	mesh->LoadColors(colors, 6);

	meshRenderer->SetMesh(mesh);

	//scene.emplaceChild(go);
	return go;
}