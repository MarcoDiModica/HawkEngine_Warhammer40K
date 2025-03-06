#include "UICanvasComponent.h"

#include "../MyGameEditor/App.h"
#include "../MyGameEditor/MyWindow.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"

#include "UIImageComponent.h"


UICanvasComponent::UICanvasComponent(GameObject* owner)	: Component(owner)
{
	name = "UICanvasComponent";
    glGenFramebuffers(1, &fboUI);
    glBindFramebuffer(GL_FRAMEBUFFER, fboUI);

    glGenTextures(1, &fboTextureUI);
    glBindTexture(GL_TEXTURE_2D, fboTextureUI);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureUI, 0);

    glGenRenderbuffers(1, &rboUI);
    glBindRenderbuffer(GL_RENDERBUFFER, rboUI);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboUI);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG(LogType::LOG_ERROR, "Game View Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UICanvasComponent::Start()
{
	
}

void UICanvasComponent::Update(float deltaTime)
{
    for (size_t i = 0; i < owner->GetChildren().size(); ++i) {
        GameObject* object = owner->GetChildren()[i].get();

		if (object->HasComponent<UIImageComponent>())
		{
			object->GetComponent<UIImageComponent>()->Update(deltaTime);
		}
    }
}

void UICanvasComponent::Destroy()
{

}

std::unique_ptr<Component> UICanvasComponent::Clone(GameObject* owner)
{
	return std::make_unique<UICanvasComponent>(owner);
}