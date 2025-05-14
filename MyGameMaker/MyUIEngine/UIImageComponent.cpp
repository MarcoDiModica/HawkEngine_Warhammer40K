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
	if (!texture) return;

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
					animIndex = anim1IndexLimit + 1;
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
	mesh = std::make_shared<Mesh>();
	mesh = Mesh::CreatePlane();
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
