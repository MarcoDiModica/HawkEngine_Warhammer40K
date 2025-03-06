#include "UIImageComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEditor/MyWindow.h"

UIImageComponent::UIImageComponent(GameObject* owner) : Component(owner)
{
	name = "UICanvasComponent";
}

void UIImageComponent::Start()
{

}

void UIImageComponent::Update(float deltaTime)
{

	shader->Bind();

	glm::vec4 color(255, 255, 255, 255);
	shader->SetUniform("modColor", color);

	if (texture->image_path != "") {
		texture->bind();
		shader->SetUniform("u_HasTexture", true);
		shader->SetUniform("texture1", 0);
	}
	else {
		shader->SetUniform("u_HasTexture", false);
	}

	glm::mat4 projectionMatrix = glm::ortho(
		0.0f, static_cast<float>(Application->window->width()),
		static_cast<float>(Application->window->height()), 0.0f,
		-1.0f, 1.0f);

	glm::mat4 viewMatrix = glm::mat4(1.0f); // Identidad porque la UI no usa vista 3D

	glm::vec3 worldPos = owner->GetComponent<Transform_Component>()->GetPosition();
	glm::vec2 scale = owner->GetComponent<Transform_Component>()->GetScale();

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos.x, worldPos.y, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

	shader->SetUniform("model", modelMatrix);
	shader->SetUniform("view", Application->root->mainCamera->GetComponent<CameraComponent>()->view());
	shader->SetUniform("projection", Application->root->mainCamera->GetComponent<CameraComponent>()->projection());

	glBindVertexArray(mesh->getModel()->GetModelData().vA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getModel()->GetModelData().iBID);

	glDrawElements(GL_TRIANGLES, mesh->getModel()->GetModelData().indexData.size(), GL_UNSIGNED_INT, nullptr);

	shader->UnBind();

	if (texture->image_path != "") {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void UIImageComponent::Destroy()
{
	shader = nullptr;
}

std::unique_ptr<Component> UIImageComponent::Clone(GameObject* owner)
{
	return std::make_unique<UIImageComponent>(owner);
}

void UIImageComponent::SetTexture(std::string path)
{
	texture = std::make_shared<Image>();
	texture->LoadTexture(path);
	shader = &Application->root->shaders[0];
	LoadMesh();
}

void UIImageComponent::LoadMesh()
{
	std::shared_ptr<Model> model = std::make_shared<Model>();

	model->GetModelData().vertexData = {
		Vertex {vec3(-1.0f, 0.0f, 1.0f)},
		Vertex {vec3(1.0f, 0.0f, 1.0f)},
		Vertex {vec3(1.0f, 0.0f, -1.0f)},
		Vertex {vec3(-1.0f, 0.0f, -1.0f)}
	};

	model->GetModelData().indexData = {
		0, 1, 2, 0, 2, 3
	};

	model->GetModelData().vertex_texCoords = {
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f)
	};

	model->SetMeshName("Plane");

	mesh = std::make_shared<Mesh>();
	mesh->setModel(model);
	mesh->loadToOpenGL();
}
