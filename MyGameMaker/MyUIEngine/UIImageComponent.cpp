#include "UIImageComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEditor/UIGameView.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UISceneWindow.h"
#include "../MyUIEngine/UITransformComponent.h"

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

	/*glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(Application->gui->UIGameViewPanel->GetWinSize().x),
		0.0f, static_cast<float>(Application->gui->UIGameViewPanel->GetWinSize().y),
		-1.0f, 1.0f);*/

	glm::mat4 projection = glm::ortho(
		0.0f, static_cast<float>(Application->gui->UIGameViewPanel->GetWinSize().x),
		static_cast<float>(Application->gui->UIGameViewPanel->GetWinSize().y), 0.0f,
		-1.0f, 1.0f);

	glm::mat4 viewMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0, 0)));

	//glm::vec3 translation = owner->GetComponent<Transform_Component>()->GetPosition();
	//glm::vec3 rotation = owner->GetComponent<Transform_Component>()->GetRotation(); // Rotación en grados
	//glm::vec3 scale = owner->GetComponent<Transform_Component>()->GetScale();
	auto rectTransform = owner->GetComponent<UITransformComponent>();

	glm::vec3 translation = rectTransform->GetPosition();
	glm::quat rotation = glm::quat(glm::vec3(glm::radians(0.0f), 0.0f, 0.0f));
	glm::vec3 scale = rectTransform->GetScale();

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translation) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::scale(glm::mat4(1.0f), scale);

	shader->SetUniform("model", modelMatrix);
	shader->SetUniform("view", viewMatrix);
	shader->SetUniform("projection", projection);

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
		Vertex {vec3(0.0f, 0.0f, 0.0f)},
		Vertex {vec3(texture->width(), 0.0f, 0.0f)},
		Vertex {vec3(texture->width(), texture->height(), 0.0f)},
		Vertex {vec3(0.0f, texture->height(), 0.0f)}
	};

	model->GetModelData().indexData = {
		0, 2, 1, 0, 3, 2
	};

	model->GetModelData().vertex_texCoords = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f)
	};

	model->SetMeshName("Plane");

	mesh = std::make_shared<Mesh>();
	mesh->setModel(model);
	mesh->loadToOpenGL();
}
