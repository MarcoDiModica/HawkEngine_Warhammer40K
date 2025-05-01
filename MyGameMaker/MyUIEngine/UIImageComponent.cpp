#include "UIImageComponent.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEditor/UIGameView.h"
#include "../MyGameEditor/MyGUI.h"
#include "../MyGameEditor/UISceneWindow.h"
#include "../MyUIEngine/UITransformComponent.h"
#include "../MyUIEngine/UICanvasComponent.h"
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"
#include "../MyGameEngine/ShaderManager.h"

UIImageComponent::UIImageComponent(GameObject* owner) : Component(owner)
{
	name = "UIImageComponent";
}

void UIImageComponent::Awake()
{
	
}

void UIImageComponent::Start()
{
	
}

int CalculateMaxIndex(const glm::vec2& sheetSize, const glm::vec2& spriteSize) {
	int columns = static_cast<int>(sheetSize.x / spriteSize.x);
	int rows = static_cast<int>(sheetSize.y / spriteSize.y);
	return columns * rows - 1;
}

glm::vec2 CalculateSpriteOffset(int index, const glm::vec2& sheetSize, const glm::vec2& spriteSize) {
	int columns = static_cast<int>(sheetSize.x / spriteSize.x);
	int rows = static_cast<int>(sheetSize.y / spriteSize.y);

	int currentColumn = index % columns;
	int currentRow = index / columns;

	float offsetX = currentColumn * spriteSize.x;
	float offsetY = currentRow * spriteSize.y;

	return glm::vec2(offsetX, offsetY);
}

void UIImageComponent::Update(float deltaTime)
{
	if (!enabled) return;
	if (shader == nullptr) return;

	auto uiTransform = owner->GetComponent<UITransformComponent>();

	if (!uiTransform->GetResised() && uiTransform->GetCanvasSize().x > 0) {

		float scaleX = 1.0f;
		float scaleY = 1.0f;

		if (texture->width() < uiTransform->GetCanvasSize().x) {
			scaleX = (texture->width() / uiTransform->GetCanvasSize().x);
		}

		if (texture->height() < uiTransform->GetCanvasSize().y) {
			scaleY = (texture->height() / uiTransform->GetCanvasSize().y);
		}

		auto scale = uiTransform->GetScale();
		uiTransform->Scale(glm::vec3(scale.x * scaleX, scale.y * scaleY, scale.z));
		uiTransform->SetResized(true);
	}

	shader->Bind();

	if (useAnimation && sheetSize != glm::vec2(0, 0))
	{
		indexTimer += deltaTime;

		if (indexTimer >= animSpeed)
		{
			if (animationNum == 0) 
			{
				if (animIndex >= anim1IndexLimit - 1)
				{
					indexTimer = 0;
					animIndex = 0;
				}
				else
				{
					if (playAnimation) 
					{
						animIndex++;
					}
					
				}
			}
			else if (animationNum == 1)
			{
				if (animIndex >= CalculateMaxIndex(sheetSize, spriteSize))
				{
					animIndex = anim1IndexLimit+ 1;
					indexTimer = 0;
				}
				else
				{
					if (playAnimation)
					{
						animIndex++;
					}
				}
			}
			
			indexTimer = 0.0f;
			spriteOffset = CalculateSpriteOffset(animIndex, sheetSize, spriteSize);

		}
	}
	else
	{
		spriteOffset = glm::vec2(0.0f, 0.0f);
		spriteSize = sheetSize;
	}

	if (texture->image_path != "") {
		texture->bind();
		shader->SetUniform("u_HasTexture", true);
		shader->SetUniform("texture1", 0);
		shader->SetUniformVec2("SpriteSize", spriteSize);
		shader->SetUniformVec2("SpriteOffset", spriteOffset);
		shader->SetUniformVec2("SheetSize", sheetSize);
	}
	else {
		shader->SetUniform("u_HasTexture", false);
	}

	glm::mat4 viewMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0, 0)));

	glm::vec3 scale = uiTransform->GetScale() * uiTransform->GetCanvasSize();
	glm::vec3 translation = uiTransform->GetCanvasPosition() + (uiTransform->GetPosition() * uiTransform->GetCanvasSize());
	glm::quat rotation = glm::quat(glm::vec3(glm::radians(0.0f), 0.0f, 0.0f));

	translation -= uiTransform->GetPivotOffset() * scale;

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translation) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::scale(glm::mat4(1.0f), scale);

	shader->SetUniformMat4("model", modelMatrix);
	shader->SetUniformMat4("view", viewMatrix);
	shader->SetUniformMat4("projection", projection);
	shader->SetUniformVec4("modColor", color);

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
	sheetSize = glm::vec2(texture->width(), texture->height());
	shader = ShaderManager::GetInstance().GetShader(ShaderType::UNLIT);
	LoadMesh();
}

void UIImageComponent::LoadMesh()
{
	std::shared_ptr<Model> model = std::make_shared<Model>();

	model->GetModelData().vertexData = {
		Vertex {vec3(0.0f, 0.0f, 0.0f)},
		Vertex {vec3(1.0f, 0.0f, 0.0f)},
		Vertex {vec3(1.0f, 1.0f, 0.0f)},
		Vertex {vec3(0.0f, 1.0f, 0.0f)}
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

MonoObject* UIImageComponent::GetSharp()
{
	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "UIImage");
	if (!klass) {
		return nullptr;
	}

	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.UIImage:.ctor(uintptr,HawkEngine.GameObject)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
	if (!method)
	{
		return nullptr;
	}

	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
	MonoObject* ownerGo = owner->GetSharp();
	if (!ownerGo)
	{
		return nullptr;
	}

	void* args[2];
	args[0] = &componentPtr;
	args[1] = ownerGo;

	mono_runtime_invoke(method, monoObject, args, nullptr);

	return monoObject;
}
